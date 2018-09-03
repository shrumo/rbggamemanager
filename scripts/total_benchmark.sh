#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tArimaa"
../build/benchmark ../rbgParser/examples/arimaa.rbg -n 2 -s 0
../build/benchmark ../rbgParser/examples/arimaa-fixedPosition.rbg -d 1

echo -e "\n\tAmazons"
../build/benchmark ../rbgParser/examples/amazons.rbg -n 2000 -s 0
../build/benchmark ../rbgParser/examples/amazons.rbg -d 2

echo -e "\n\tBreakthrough"
../build/benchmark ../rbgParser/examples/breakthrough.rbg -n 100000 -s 0
../build/benchmark ../rbgParser/examples/breakthrough.rbg -d 5

echo -e "\n\tChess"
../build/benchmark ../rbgParser/examples/chess.rbg -n 1000 -s 0
../build/benchmark ../rbgParser/examples/chess.rbg -d 5

echo -e "\n\tChess-KingCapture"
../build/benchmark ../rbgParser/examples/chess-kingCapture.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/chess-kingCapture.rbg  -d 5

echo -e "\n\tChineseCheckers6"
../build/benchmark ../rbgParser/examples/chineseCheckers6.rbg -n 1000 -s 0
../build/benchmark ../rbgParser/examples/chineseCheckers6.rbg -d 4

echo -e "\n\tConnect4"
../build/benchmark ../rbgParser/examples/connect4.rbg -n 100000 -s 0
../build/benchmark ../rbgParser/examples/connect4.rbg -d 8

echo -e "\n\tDoubleChess"
../build/benchmark ../rbgParser/examples/doubleChess.rbg -n 100 -s 0
../build/benchmark ../rbgParser/examples/doubleChess.rbg -d 3

echo -e "\n\tEnglishCheckers"
../build/benchmark ../rbgParser/examples/englishCheckers.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/englishCheckers.rbg -d 8

echo -e "\n\tEnglishCheckers-split"
../build/benchmark ../rbgParser/examples/englishCheckers-split.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/englishCheckers-split.rbg -d 8

echo -e "\n\tGo"
../build/benchmark ../rbgParser/examples/go.rbg -n 200 -s 0
../build/benchmark ../rbgParser/examples/go.rbg -d 2

echo -e "\n\tGomoku"
../build/benchmark ../rbgParser/examples/gomoku.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/gomoku.rbg -d 3

echo -e "\n\tHex"
../build/benchmark ../rbgParser/examples/hex.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/hex.rbg -d 3

echo -e "\n\tInternationalCheckers"
../build/benchmark ../rbgParser/examples/internationalCheckers.rbg -n 1000 -s 0
../build/benchmark ../rbgParser/examples/internationalCheckers.rbg -d 6

echo -e "\n\tPaperSoccer"
../build/benchmark ../rbgParser/examples/paperSoccer.rbg -n 100 -s 0
../build/benchmark ../rbgParser/examples/paperSoccer.rbg -d 5

echo -e "\n\tReversi"
../build/benchmark ../rbgParser/examples/reversi.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/reversi.rbg -d 8

echo -e "\n\tTheMillGame"
../build/benchmark ../rbgParser/examples/theMillGame.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/theMillGame.rbg -d 5

echo -e "\n\tTicTacToe"
../build/benchmark ../rbgParser/examples/ticTacToe.rbg -n 1000000 -s 0
../build/benchmark ../rbgParser/examples/ticTacToe.rbg -d 10

	#Arimaa
#Preprocessing took: 0.153796s
#Calculated 2 games in 233.212226s
#Time for one game: 116.606113s (0.008576 games/sec)
#Avarage number of turns in game: 141.500000
#Avarage number of moves in one state_: 194619.000000
#Number of traveled player states: 283 (1.213487 states/sec)
#Number of traveled keeper states: 2048 (8.781701 states/sec)
#Number of traveled states: 2331 (9.995188 states/sec)
#Number of calculated moves: 55151057 (236484.415678 moves/sec)
#Avarage player scores: 
	#silver : 50.000000 (Min: 0, Max: 100)
	#gold : 50.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.14237s
#Calculated fast perft for depth 1 in 1.403183s
#There are 20652 leaves
#Number of traveled states: 20653 (14718.678270 states/sec)

	#Amazons
#Preprocessing took: 0.0245159s
#Calculated 2000 games in 7.826200s
#Time for one game: 0.003913s (255.551867 games/sec)
#Avarage number of turns in game: 72.354000
#Avarage number of moves in one state_: 452.848000
#Number of traveled player states: 144708 (18490.199820 states/sec)
#Number of traveled keeper states: 142708 (18234.647952 states/sec)
#Number of traveled states: 287416 (36724.847772 states/sec)
#Number of calculated moves: 65243172 (8336507.222575 moves/sec)
#Avarage player scores: 
	#white : 51.800000 (Min: 0, Max: 100)
	#black : 48.200000 (Min: 0, Max: 100)
#Preprocessing took: 0.00802082s
#Calculated fast perft for depth 2 in 0.968200s
#There are 4307152 leaves
#Number of traveled states: 4309329 (4450865.729027 states/sec)

	#Breakthrough
#Preprocessing took: 0.0134712s
#Calculated 100000 games in 23.574441s
#Time for one game: 0.000236s (4241.882121 games/sec)
#Avarage number of turns in game: 63.999340
#Avarage number of moves in one state_: 25.348570
#Number of traveled player states: 6399934 (271477.656106 states/sec)
#Number of traveled keeper states: 6499934 (275719.538227 states/sec)
#Number of traveled states: 12899868 (547197.194334 states/sec)
#Number of calculated moves: 164493591 (6977624.226937 moves/sec)
#Avarage player scores: 
	#white : 51.078000 (Min: 0, Max: 100)
	#black : 48.922000 (Min: 0, Max: 100)
#Preprocessing took: 0.00180213s
#Calculated fast perft for depth 5 in 1.824033s
#There are 6182818 leaves
#Number of traveled states: 6450493 (3536390.813436 states/sec)

	#Chess
#Preprocessing took: 0.101105s
#Calculated 1000 games in 16.102119s
#Time for one game: 0.016102s (62.103629 games/sec)
#Avarage number of turns in game: 191.013000
#Avarage number of moves in one state_: 27.346000
#Number of traveled player states: 191013 (11862.600403 states/sec)
#Number of traveled keeper states: 382580 (23759.606216 states/sec)
#Number of traveled states: 573593 (35622.206618 states/sec)
#Number of calculated moves: 5301142 (329220.153727 moves/sec)
#Avarage player scores: 
	#white : 50.700000 (Min: 0, Max: 100)
	#black : 49.300000 (Min: 0, Max: 100)
#Preprocessing took: 0.1055s
#Calculated fast perft for depth 5 in 23.524886s
#There are 4865609 leaves
#Number of traveled states: 5072213 (215610.528066 states/sec)

	#Chess-KingCapture
#Preprocessing took: 0.0330209s
#Calculated 10000 games in 16.345388s
#Time for one game: 0.001635s (611.793380 games/sec)
#Avarage number of turns in game: 109.228300
#Avarage number of moves in one state_: 32.883200
#Number of traveled player states: 1092283 (66825.150849 states/sec)
#Number of traveled keeper states: 1101110 (67365.180865 states/sec)
#Number of traveled states: 2193393 (134190.331714 states/sec)
#Number of calculated moves: 36849545 (2254430.768697 moves/sec)
#Avarage player scores: 
	#white : 50.045000 (Min: 0, Max: 100)
	#black : 49.955000 (Min: 0, Max: 100)
#Preprocessing took: 0.0175952s
#Calculated fast perft for depth 5 in 3.722129s
#There are 4896795 leaves
#Number of traveled states: 5104321 (1371344.333625 states/sec)

	#ChineseCheckers6
#Preprocessing took: 0.0281471s
#Calculated 1000 games in 4.863642s
#Time for one game: 0.004864s (205.607238 games/sec)
#Avarage number of turns in game: 201.000000
#Avarage number of moves in one state_: 53.396000
#Number of traveled player states: 201000 (41327.054775 states/sec)
#Number of traveled keeper states: 201000 (41327.054775 states/sec)
#Number of traveled states: 402000 (82654.109549 states/sec)
#Number of calculated moves: 10830822 (2226895.393281 moves/sec)
#Avarage player scores: 
	#southWestPlayer : 50.000000 (Min: 50, Max: 50)
	#southPlayer : 50.000000 (Min: 50, Max: 50)
	#southEastPlayer : 50.000000 (Min: 50, Max: 50)
	#northWestPlayer : 50.000000 (Min: 50, Max: 50)
	#northPlayer : 50.000000 (Min: 50, Max: 50)
	#northEastPlayer : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0143611s
#Calculated fast perft for depth 4 in 0.237499s
#There are 336969 leaves
#Number of traveled states: 351541 (1480180.390840 states/sec)

	#Connect4
#Preprocessing took: 0.0199643s
#Calculated 100000 games in 3.041387s
#Time for one game: 0.000030s (32879.736576 games/sec)
#Avarage number of turns in game: 21.324400
#Avarage number of moves in one state_: 6.537100
#Number of traveled player states: 2132440 (701140.654648 states/sec)
#Number of traveled keeper states: 2231898 (733842.183052 states/sec)
#Number of traveled states: 4364338 (1434982.837700 states/sec)
#Number of calculated moves: 14408733 (4737553.454384 moves/sec)
#Avarage player scores: 
	#yellow : 44.641500 (Min: 0, Max: 100)
	#red : 55.358500 (Min: 0, Max: 100)
#Preprocessing took: 0.00800432s
#Calculated fast perft for depth 8 in 4.192594s
#There are 5628804 leaves
#Number of traveled states: 6634027 (1582320.397149 states/sec)

	#DoubleChess
#Preprocessing took: 0.178855s
#Calculated 100 games in 22.449950s
#Time for one game: 0.224500s (4.454353 games/sec)
#Avarage number of turns in game: 371.050000
#Avarage number of moves in one state_: 93.140000
#Number of traveled player states: 37105 (1652.787618 states/sec)
#Number of traveled keeper states: 74245 (3307.134258 states/sec)
#Number of traveled states: 111350 (4959.921876 states/sec)
#Number of calculated moves: 3463185 (154262.479049 moves/sec)
#Avarage player scores: 
	#white : 47.000000 (Min: 0, Max: 100)
	#black : 53.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.163034s
#Calculated fast perft for depth 3 in 3.693500s
#There are 389450 leaves
#Number of traveled states: 394707 (106865.310741 states/sec)

	#EnglishCheckers
#Preprocessing took: 0.0235825s
#Calculated 10000 games in 3.906522s
#Time for one game: 0.000391s (2559.821973 games/sec)
#Avarage number of turns in game: 67.789300
#Avarage number of moves in one state_: 4.613300
#Number of traveled player states: 677893 (173528.539654 states/sec)
#Number of traveled keeper states: 1338570 (342650.089799 states/sec)
#Number of traveled states: 2016463 (516178.629453 states/sec)
#Number of calculated moves: 3497279 (895241.162885 moves/sec)
#Avarage player scores: 
	#white : 50.420000 (Min: 0, Max: 100)
	#black : 49.580000 (Min: 0, Max: 100)
#Preprocessing took: 0.0143352s
#Calculated fast perft for depth 8 in 1.823732s
#There are 845931 leaves
#Number of traveled states: 1071628 (587601.647782 states/sec)

	#EnglishCheckers-split
#Preprocessing took: 0.0224007s
#Calculated 10000 games in 4.349879s
#Time for one game: 0.000435s (2298.914323 games/sec)
#Avarage number of turns in game: 70.046500
#Avarage number of moves in one state_: 4.458100
#Number of traveled player states: 700465 (161030.902150 states/sec)
#Number of traveled keeper states: 1013377 (232966.690025 states/sec)
#Number of traveled states: 1713842 (393997.592175 states/sec)
#Number of calculated moves: 3524188 (810180.627135 moves/sec)
#Avarage player scores: 
	#white : 51.615000 (Min: 0, Max: 100)
	#black : 48.385000 (Min: 0, Max: 100)
#Preprocessing took: 0.0144957s
#Calculated fast perft for depth 8 in 1.824816s
#There are 838248 leaves
#Number of traveled states: 1063460 (582776.444151 states/sec)

	#Go
#Preprocessing took: 0.0193954s
#Calculated 200 games in 7.086259s
#Time for one game: 0.035431s (28.223637 games/sec)
#Avarage number of turns in game: 474.335000
#Avarage number of moves in one state_: 148.615000
#Number of traveled player states: 94867 (13387.459081 states/sec)
#Number of traveled keeper states: 784098 (110650.488474 states/sec)
#Number of traveled states: 878965 (124037.947555 states/sec)
#Number of calculated moves: 13922430 (1964708.085279 moves/sec)
#Avarage player scores: 
	#white : 180.230000 (Min: 72, Max: 299)
	#black : 173.935000 (Min: 56, Max: 284)
#Preprocessing took: 0.00846716s
#Calculated fast perft for depth 2 in 1.259215s
#There are 130682 leaves
#Number of traveled states: 131046 (104069.573214 states/sec)

	#Gomoku
#Preprocessing took: 0.0158195s
#Calculated 10000 games in 8.853177s
#Time for one game: 0.000885s (1129.538056 games/sec)
#Avarage number of turns in game: 118.450900
#Avarage number of moves in one state_: 166.025200
#Number of traveled player states: 1184509 (133794.799299 states/sec)
#Number of traveled keeper states: 1194509 (134924.337355 states/sec)
#Number of traveled states: 2379018 (268719.136653 states/sec)
#Number of calculated moves: 193096729 (21811010.386405 moves/sec)
#Avarage player scores: 
	#white : 50.130000 (Min: 0, Max: 100)
	#black : 49.870000 (Min: 0, Max: 100)
#Preprocessing took: 0.00546264s
#Calculated fast perft for depth 3 in 7.065566s
#There are 11239200 leaves
#Number of traveled states: 11289826 (1597865.681639 states/sec)

	#Hex
#Preprocessing took: 0.00372748s
#Calculated 10000 games in 4.419652s
#Time for one game: 0.000442s (2262.621398 games/sec)
#Avarage number of turns in game: 107.490900
#Avarage number of moves in one state_: 67.514600
#Number of traveled player states: 1074909 (243211.210395 states/sec)
#Number of traveled keeper states: 1084909 (245473.831793 states/sec)
#Number of traveled states: 2159818 (488685.042187 states/sec)
#Number of calculated moves: 72256109 (16348821.833580 moves/sec)
#Avarage player scores: 
	#red : 52.010000 (Min: 0, Max: 100)
	#blue : 47.990000 (Min: 0, Max: 100)
#Preprocessing took: 0.00486864s
#Calculated fast perft for depth 3 in 0.469011s
#There are 1727880 leaves
#Number of traveled states: 1742522 (3715310.971216 states/sec)

	#InternationalCheckers
#Preprocessing took: 0.0239554s
#Calculated 1000 games in 4.934283s
#Time for one game: 0.004934s (202.663692 games/sec)
#Avarage number of turns in game: 93.638000
#Avarage number of moves in one state_: 7.493000
#Number of traveled player states: 93638 (18977.022795 states/sec)
#Number of traveled keeper states: 277935 (56327.333246 states/sec)
#Number of traveled states: 371573 (75304.356041 states/sec)
#Number of calculated moves: 745997 (151186.506269 moves/sec)
#Avarage player scores: 
	#white : 48.350000 (Min: 0, Max: 100)
	#black : 51.650000 (Min: 0, Max: 100)
#Preprocessing took: 0.0172745s
#Calculated fast perft for depth 6 in 2.349251s
#There are 167140 leaves
#Number of traveled states: 199271 (84823.191814 states/sec)

	#PaperSoccer
#Preprocessing took: 0.0286764s
#Calculated 100 games in 3.173989s
#Time for one game: 0.031740s (31.506099 games/sec)
#Avarage number of turns in game: 58.450000
#Avarage number of moves in one state_: 140.180000
#Number of traveled player states: 5845 (1841.531497 states/sec)
#Number of traveled keeper states: 5745 (1810.025398 states/sec)
#Number of traveled states: 11590 (3651.556895 states/sec)
#Number of calculated moves: 825052 (259941.701391 moves/sec)
#Avarage player scores: 
	#white : 45.000000 (Min: 0, Max: 100)
	#black : 55.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0197409s
#Calculated fast perft for depth 5 in 0.176215s
#There are 82026 leaves
#Number of traveled states: 88081 (499849.564468 states/sec)

	#Reversi
#Preprocessing took: 0.0114034s
#Calculated 10000 games in 9.202126s
#Time for one game: 0.000920s (1086.705372 games/sec)
#Avarage number of turns in game: 60.414200
#Avarage number of moves in one state_: 7.982800
#Number of traveled player states: 604142 (65652.435690 states/sec)
#Number of traveled keeper states: 624142 (67825.846434 states/sec)
#Number of traveled states: 1228284 (133478.282124 states/sec)
#Number of calculated moves: 5120300 (556425.751666 moves/sec)
#Avarage player scores: 
	#white : 51.900000 (Min: 0, Max: 100)
	#black : 48.100000 (Min: 0, Max: 100)
#Preprocessing took: 0.0143959s
#Calculated fast perft for depth 8 in 2.580401s
#There are 390216 leaves
#Number of traveled states: 455221 (176414.832240 states/sec)

	#TheMillGame
#Preprocessing took: 0.012892s
#Calculated 10000 games in 11.504438s
#Time for one game: 0.001150s (869.229734 games/sec)
#Avarage number of turns in game: 51.000000
#Avarage number of moves in one state_: 20.781100
#Number of traveled player states: 510000 (44330.716420 states/sec)
#Number of traveled keeper states: 1093982 (95092.168255 states/sec)
#Number of traveled states: 1603982 (139422.884675 states/sec)
#Number of calculated moves: 10847476 (942894.867497 moves/sec)
#Avarage player scores: 
	#white : 50.000000 (Min: 50, Max: 50)
	#black : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0146485s
#Calculated fast perft for depth 5 in 5.598007s
#There are 5140800 leaves
#Number of traveled states: 5408545 (966155.485689 states/sec)

	#TicTacToe
#Preprocessing took: 0.00797047s
#Calculated 1000000 games in 6.567567s
#Time for one game: 0.000007s (152263.394831 games/sec)
#Avarage number of turns in game: 7.753578
#Avarage number of moves in one state_: 5.415552
#Number of traveled player states: 7753578 (1180586.108369 states/sec)
#Number of traveled keeper states: 8499784 (1294205.967172 states/sec)
#Number of traveled states: 16253362 (2474792.075541 states/sec)
#Number of calculated moves: 42527080 (6475317.573060 moves/sec)
#Avarage player scores: 
	#xplayer : 64.813050 (Min: 0, Max: 100)
	#oplayer : 35.186950 (Min: 0, Max: 100)
#Preprocessing took: 0.00200943s
#Calculated fast perft for depth 10 in 0.305772s
#There are 0 leaves
#Number of traveled states: 549946 (1798551.828043 states/sec)
