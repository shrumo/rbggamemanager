#! /usr/bin/python3
from tkinter import *


def _dist(a, b):
    return ((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2) ** (1 / 2)


def _mult(c, v):
    return c * v[0], c * v[1]


def _add(a, b):
    return a[0] + b[0], a[1] + b[1]


# Uses De Casteljau algorithm to calculate some points of the bezier curve
def _bezier(points, percent=None, accuracy=12):
    alphas = [percent]
    if percent is None:
        alphas = [i / (accuracy - 1) for i in range(accuracy)]
    result = []
    for alpha in alphas:
        current = points
        while len(current) > 1:
            next = [_add(_mult(1 - alpha, a), _mult(alpha, b))
                    for a, b in zip(current, current[1:])]
            current = next
        result += current
    return result if percent is None else result[0]


class Graph(Canvas):
    default_node_size = 100
    recommended_node_distance = 400
    default_highlight_color = "lightyellow"
    default_node_color = "white"
    default_node_outline_color = "black"
    default_background_color = "white"

    def add_node_abs(self, x, y, text=''):
        canvas_x, canvas_y = self._canvas_pos((x, y))
        return self.add_node_rel(canvas_x, canvas_y, text)

    def add_node_rel(self, x, y, text=''):
        node_id = self.create_oval(x - self._node_size() / 2, y - self._node_size() / 2,
                                   x + self._node_size() / 2, y + self._node_size() / 2,
                                   fill=Graph.default_node_color, outline=Graph.default_node_outline_color,
                                   tags='node')
        node_label = self.create_text(x, y, text=text, tags='text')
        self._nodes[node_id] = node_label,
        self._nodes_edges[node_id] = set()
        return node_id

    def change_node_label(self, node_id, text):
        self.delete(self._nodes[node_id])
        x, y = self.coords(node_id)[:2]
        x += self._node_size() / 2
        y += self._node_size() / 2
        node_label = self.create_text(x, y, text=text, tags='text')
        self._nodes[node_id] = node_label

    def add_edge(self, a, b, text='', refresh_paired=True, **kwargs):
        if (a, b) in self._nodes_edges:
            previous_edge = self._nodes_edges[(a, b)]
            text = self._edges[previous_edge][3] + ', ' + text
            self._delete_edge(previous_edge)
        edge_id = self.create_line(self._edge_points(
            a, b), smooth=True, arrow="last", **kwargs)
        edge_label = self.create_text(
            *self._edge_text_point(a, b), text=text, tags='text')
        self._nodes_edges[a] |= {edge_id}
        self._nodes_edges[b] |= {edge_id}
        self._nodes_edges[(a, b)] = edge_id
        self._edges[edge_id] = (a, b, edge_label, text, kwargs)
        if refresh_paired and (b, a) in self._nodes_edges:
            self._recreate_edge(
                self._nodes_edges[(b, a)], refresh_paired=False)

    def set_node_clicked_callback(self, callback):
        self._node_clicked_callback = callback

    def unselect(self):
        self._change_node_highlight(self._last_clicked_item, False)
        self._last_clicked_item = None

    def _canvas_pos(self, pos):
        return self.canvasx(pos[0]), self.canvasy(pos[1])

    def __init__(self, parent):
        super().__init__(parent)

        self.configure(background=Graph.default_background_color)

        self._scale_ratio = 1
        # This alows zoom to work on Windows
        self.bind("<MouseWheel>", lambda e: self._zoom(e.delta))

        # This allows zoom to work on Linux
        self.bind("<Button-4>", lambda e: self._zoom(1, (e.x, e.y)))
        self.bind("<Button-5>", lambda e: self._zoom(-1, (e.x, e.y)))

        self.grabbed_item = None
        self.bind("<Button-1>", self._on_click)
        self.bind("<B1-Motion>", self._on_drag)
        self.bind("<ButtonRelease-1>", self._on_drop)

        self._nodes = {}
        self._last_clicked_item = None

        self._edges = {}
        self._nodes_edges = {}

        self._node_clicked_callback = lambda _: None

    # Returns the node size in pixel in current scale ratio
    def _node_size(self):
        return self._scale_ratio * Graph.default_node_size

    # Calculates the coordinates of the control points for the bezier curve so that the edges look nice
    def _edge_control_points(self, a_id, b_id):
        paired = (b_id, a_id) in self._nodes_edges

        begin_shift = (self._node_size() / 2, 0)
        bend_shift = (self._node_size() * 2 ** (1 / 2), 0)

        if paired:
            begin_shift = (2 ** (1 / 2) / 2 * self._node_size() / 2,
                           2 ** (1 / 2) / 2 * self._node_size() / 2)
            bend_shift = (self._node_size(), self._node_size())

        a = self.coords(a_id)[0] + self._node_size() / \
            2, self.coords(a_id)[1] + self._node_size() / 2
        b = self.coords(b_id)[0] + self._node_size() / \
            2, self.coords(b_id)[1] + self._node_size() / 2
        if b[0] > a[0]:
            return [_add(a, (begin_shift[0], -begin_shift[1])),
                    (a[0] + bend_shift[0], a[1] - bend_shift[1]),
                    (b[0] - bend_shift[0], b[1] - bend_shift[1]),
                    _add(b, (-begin_shift[0], -begin_shift[1]))]
        else:
            return [_add(a, (-begin_shift[0], begin_shift[1])),
                    (a[0] - bend_shift[0], a[1] + bend_shift[1]),
                    (b[0] + bend_shift[0], b[1] + bend_shift[1]),
                    _add(b, (begin_shift[0], begin_shift[1]))]

    # Calculates the points of the edge
    def _edge_points(self, a_id, b_id):
        return _bezier(self._edge_control_points(a_id, b_id))

    # Calculates where the text should be put on the edge
    def _edge_text_point(self, a_id, b_id):
        a = self.coords(a_id)
        b = self.coords(b_id)
        mid_point_first = _bezier(
            self._edge_control_points(a_id, b_id), percent=0.48)
        mid_point_second = _bezier(
            self._edge_control_points(a_id, b_id), percent=0.52)
        direction = _add(mid_point_first, _mult(-1, mid_point_second))
        direction = _mult(1 / _dist((0, 0), direction), direction)
        perpendicular = (-direction[1], direction[0])
        mid_point = _bezier(self._edge_control_points(a_id, b_id), percent=0.5)
        if b[0] > a[0]:
            return _add(mid_point, _mult(self._node_size() / 4, perpendicular))
        else:
            return _add(mid_point, _mult(-self._node_size() / 4, perpendicular))

    # Left mouse button click
    def _on_click(self, event):
        if self._grab_node(event):
            return
        self._scroll_screen_start(event)

    def _scroll_screen_start(self, event):
        self.scan_mark(event.x, event.y)
        return True

    def _grab_node(self, event):
        event_canv_pos = (event_canv_x, event_canv_y) = self._canvas_pos(
            (event.x, event.y))
        ids = self.find_overlapping(event_canv_x - self._node_size() / 2,
                                    event_canv_y - self._node_size() / 2,
                                    event_canv_x + self._node_size() / 2,
                                    event_canv_y + self._node_size() / 2)
        nodes_ids = [id for id in ids if id in self._nodes]
        if len(nodes_ids) == 0:
            return False
        distance, closest_node = min(((_dist(self.coords(id), event_canv_pos), id) for id in nodes_ids),
                                     key=lambda x: (x[0], -x[1]))
        if distance > self._node_size():
            return False
        self.grabbed_item = closest_node
        if self._last_clicked_item:
            self._change_node_highlight(self._last_clicked_item, False)
        self._last_clicked_item = self.grabbed_item
        self._change_node_highlight(self._last_clicked_item, True)
        self._node_clicked_callback(self.grabbed_item)
        return True

    # Left mouse button drag
    def _on_drag(self, event):
        if self._drag_node(event):
            return
        self._drag_screen(event)

    def _move_node(self, node_id, pos):
        self.coords(node_id, pos[0] - self._node_size() / 2,
                    pos[1] - self._node_size() / 2,
                    pos[0] + self._node_size() / 2, pos[1] + self._node_size() / 2)
        label_id = self._nodes[node_id]
        self.coords(label_id, pos[0], pos[1])

    def _drag_node(self, event):
        if self.grabbed_item:
            self._move_node(self.grabbed_item,
                            self._canvas_pos((event.x, event.y)))
            for edge in self._nodes_edges[self.grabbed_item]:
                self._recreate_edge(edge)
            return True
        return False

    def _drag_screen(self, event):
        self.scan_dragto(event.x, event.y, gain=1)
        return True

    # Left mouse button release
    def _on_drop(self, event):
        self.grabbed_item = None

    def _delete_edge(self, edge_id):
        a, b, label_id, label, _ = self._edges[edge_id]
        self._nodes_edges[a] -= {edge_id}
        self._nodes_edges[b] -= {edge_id}
        self._nodes_edges.pop((a, b), None)
        self._edges.pop(edge_id, None)
        self.delete(edge_id)
        self.delete(label_id)

    def _recreate_edge(self, edge_id, refresh_paired=True):
        a, b, _, label, kwargs = self._edges[edge_id]
        self._delete_edge(edge_id)
        self.add_edge(a, b, label, refresh_paired, **kwargs)

    def _change_node_highlight(self, id, highlight):
        if highlight:
            self.itemconfigure(id, fill=Graph.default_highlight_color)
        else:
            self.itemconfigure(id, fill=Graph.default_node_color)

    def _zoom(self, delta, pos=(0, 0)):
        if delta > 0:
            self.scale("all", *self._canvas_pos(pos), 1.1, 1.1)

            self._scale_ratio = self._scale_ratio * 1.1
        elif delta < 0:
            self.scale("all", *self._canvas_pos(pos), 0.9, 0.9)
            self._scale_ratio = self._scale_ratio * 0.9


def main():
    root = Tk()

    root.title("graph_widget example")

    root.geometry("800x600")

    Grid.rowconfigure(root, 0, weight=1)
    Grid.columnconfigure(root, 0, weight=1)
    graph = Graph(root)
    graph.grid(column=0, row=0, sticky=N + S + E + W)

    label = Label(root, text="Nothing selected")
    label.grid(column=1, row=0)

    graph.set_node_clicked_callback(
        lambda x: label.configure(text="Selected " + str(x)))

    last_node_label = None

    for y in range(5):
        for x in range(2):
            i = x + y * 5
            a = graph.add_node_rel(x * Graph.recommended_node_distance, y * Graph.recommended_node_distance,
                                   'a' + str(i))
            b = graph.add_node_rel(x * Graph.recommended_node_distance * 2 + Graph.recommended_node_distance,
                                   y * Graph.recommended_node_distance, 'b' + str(i))
            graph.add_edge(a, b, "e" + str(i))
            if i % 3 == 0:
                graph.add_edge(b, a, "e" + str(i) + "rev")
            if i % 7 == 0:
                graph.add_edge(b, a, "e" + str(i) + "rev", dash=(2, 8))
            last_node_label = b

    graph.change_node_label(last_node_label, "last")

    root.mainloop()


if __name__ == '__main__':
    main()
