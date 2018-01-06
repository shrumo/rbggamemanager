#!/usr/bin/python3
import subprocess
import math
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("filename")
parser.add_argument("-n","--number",type=int,default=1)
args = parser.parse_args()

def make_project():
    out = subprocess.check_output(["./make_me.sh"])
    out = out.decode("utf-8")
    return "Done." in out

exec_path = "../build/rbggamemanager"

def make_play(game_filename,seed=0):
    out = subprocess.check_output([exec_path,game_filename,"-t","-s","-r","--randomseed",str(seed)])
    out = out.decode("utf-8")
    parsed = 0
    played = 0
    scores = {}
    for line in out.split("\n"): 
        if "parsed" in line:
            parsed = float(line.split(":")[-1][:-1].strip())
        if "played" in line:
            played = float(line.split(":")[-1][:-1].strip())
        if "\t" in line:
            player, score = map(lambda x : x.strip(), line.split(":"))
            score = float(score)
            scores[player] = score
    return (parsed,played,scores)

def get_players(game_filename):
    f = open(game_filename)
    for line in f:
        if "#players" in line:
            return [x.strip() for x in line.split("=")[-1].split(",")]

if not make_project():
    print("Couldn't make project.")
    exit(1)

print("Project made.")

parsing_time_sum = 0
playing_time_sum = 0
scores_sum = {}
scores_min = {}
scores_max = {}
for player in get_players(args.filename):
    scores_sum[player] = 0
    scores_min[player] = math.inf
    scores_max[player] = -math.inf

n = args.number

for i in range(1,n+1):
    print("Doing",i,"simulation.")
    result = make_play(args.filename,i)
    parsing_time_sum += result[0]
    playing_time_sum += result[1]
    for player,points in result[2].items():
        scores_sum[player] += points
        if points > scores_max[player]:
            scores_max[player] = points
        if points < scores_min[player]:
            scores_min[player] = points

print("Parsing time avg: ",parsing_time_sum / n)
print("Playing time avg: ",playing_time_sum / n)
print("Scores:")
for player, points in scores_sum.items():
    print("\t",player,"min:",scores_min[player],"max:",scores_max[player],"avg:",scores_sum[player] / n)
