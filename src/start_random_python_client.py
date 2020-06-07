from rbg import *
import argparse

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("host",help="The host to connect to.")
  parser.add_argument("port",help="The port to connect to.")
  args = parser.parse_args()
  client = Client(args.host, args.port)
  state = CreateGameState(client.description())
  client.Ready()
  player = client.player()
  print('I am player',state.declarations().players_resolver().Name(player))
  moves = state.Moves()
  while moves:
    if state.current_player() == player:
      client.ReadDeadline()
      move = moves[0]
      state.Apply(move)
      client.Write(move)
    else:
      move = client.Read()
      state.Apply(move)
    moves = state.Moves()

if __name__=="__main__":
  main()
