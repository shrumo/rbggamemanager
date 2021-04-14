from rbg import *
import random
import argparse

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("host",help="The host to connect to.")
  parser.add_argument("port",help="The port to connect to.")
  parser.add_argument("--games_to_play",help="Time after the client will disconnect.",default=-1,type=int)
  args = parser.parse_args()
  client = Client(args.host, args.port)
  state = CreateGameState(client.description())
  client.Ready()
  player = client.player()
  print('I am player',state.declarations().players_resolver().Name(player))

  games_to_play = args.games_to_play
  moves = state.Moves()
  while moves and games_to_play != 0:
    if state.current_player() == player:
      client.ReadDeadline()
      move = moves[random.randint(0,len(moves)-1)]
      state.Apply(move)
      client.Write(move)
    else:
      move = client.Read()
      state.Apply(move)
    moves = state.Moves()
    if not moves:
      games_to_play -= 1
      client.ReadReset()
      state.Reset()
      moves = state.Moves()

if __name__=="__main__":
  main()
