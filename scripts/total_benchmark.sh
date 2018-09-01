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
#Preprocessing took: 0.142535s
#Calculated 2 games in 223.941890s
#Time for one game: 111.970945s (0.008931 games/sec)
#Avarage number of turns in game: 141.500000
#Avarage number of moves in one state_: 194619.000000
#Number of traveled player states: 283 (1.263721 states/sec)
#Number of traveled keeper states: 2048 (9.145230 states/sec)
#Number of traveled states: 2331 (10.408950 states/sec)
#Number of calculated moves: 55151057 (246273.964447 moves/sec)
#Avarage player scores: 
	#silver : 50.000000 (Min: 0, Max: 100)
	#gold : 50.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.135568s
#Calculated fast perft for depth 1 in 1.415770s
#There are 20652 leaves
#Number of traveled states: 20653 (14587.824590 states/sec)

	#Amazons
#Preprocessing took: 0.00297909s
#Calculated 2000 games in 7.735380s
#Time for one game: 0.003868s (258.552248 games/sec)
#Avarage number of turns in game: 72.354000
#Avarage number of moves in one state_: 452.848000
#Number of traveled player states: 144708 (18707.289370 states/sec)
#Number of traveled keeper states: 142708 (18448.737122 states/sec)
#Number of traveled states: 287416 (37156.026492 states/sec)
#Number of calculated moves: 65243172 (8434384.401980 moves/sec)
#Avarage player scores: 
	#white : 51.800000 (Min: 0, Max: 100)
	#black : 48.200000 (Min: 0, Max: 100)
#Preprocessing took: 0.00298006s
#Calculated fast perft for depth 2 in 1.096396s
#There are 4307152 leaves
#Number of traveled states: 4309329 (3930449.661366 states/sec)

	#Breakthrough
#Preprocessing took: 0.00180577s
#Calculated 100000 games in 25.443072s
#Time for one game: 0.000254s (3930.343047 games/sec)
#Avarage number of turns in game: 63.999340
#Avarage number of moves in one state_: 25.348570
#Number of traveled player states: 6399934 (251539.360978 states/sec)
#Number of traveled keeper states: 6499934 (255469.704025 states/sec)
#Number of traveled states: 12899868 (507009.065003 states/sec)
#Number of calculated moves: 164493591 (6465162.416531 moves/sec)
#Avarage player scores: 
	#white : 51.078000 (Min: 0, Max: 100)
	#black : 48.922000 (Min: 0, Max: 100)
#Preprocessing took: 0.00257392s
#Calculated fast perft for depth 5 in 1.982029s
#There are 6182818 leaves
#Number of traveled states: 6450493 (3254490.224733 states/sec)

	#Chess
#Preprocessing took: 0.101964s
#Calculated 1000 games in 16.341412s
#Time for one game: 0.016341s (61.194224 games/sec)
#Avarage number of turns in game: 191.013000
#Avarage number of moves in one state_: 27.346000
#Number of traveled player states: 191013 (11688.892336 states/sec)
#Number of traveled keeper states: 382580 (23411.686272 states/sec)
#Number of traveled states: 573593 (35100.578607 states/sec)
#Number of calculated moves: 5301142 (324399.271749 moves/sec)
#Avarage player scores: 
	#white : 50.700000 (Min: 0, Max: 100)
	#black : 49.300000 (Min: 0, Max: 100)
#Preprocessing took: 0.102289s
#Calculated fast perft for depth 5 in 23.803139s
#There are 4865609 leaves
#Number of traveled states: 5072213 (213090.090571 states/sec)

	#Chess-KingCapture
#Preprocessing took: 0.0206587s
#Calculated 10000 games in 17.642985s
#Time for one game: 0.001764s (566.797522 games/sec)
#Avarage number of turns in game: 109.228300
#Avarage number of moves in one state_: 32.883200
#Number of traveled player states: 1092283 (61910.329719 states/sec)
#Number of traveled keeper states: 1101110 (62410.641891 states/sec)
#Number of traveled states: 2193393 (124320.971610 states/sec)
#Number of calculated moves: 36849545 (2088623.077477 moves/sec)
#Avarage player scores: 
	#white : 50.045000 (Min: 0, Max: 100)
	#black : 49.955000 (Min: 0, Max: 100)
#Preprocessing took: 0.0205992s
#Calculated fast perft for depth 5 in 3.939046s
#There are 4896795 leaves
#Number of traveled states: 5103860 (1295709.605717 states/sec)

	#ChineseCheckers6
#Preprocessing took: 0.00700591s
#Calculated 1000 games in 4.940438s
#Time for one game: 0.004940s (202.411183 games/sec)
#Avarage number of turns in game: 201.000000
#Avarage number of moves in one state_: 53.396000
#Number of traveled player states: 201000 (40684.647853 states/sec)
#Number of traveled keeper states: 201000 (40684.647853 states/sec)
#Number of traveled states: 402000 (81369.295705 states/sec)
#Number of calculated moves: 10830822 (2192279.497637 moves/sec)
#Avarage player scores: 
	#southWestPlayer : 50.000000 (Min: 50, Max: 50)
	#southPlayer : 50.000000 (Min: 50, Max: 50)
	#southEastPlayer : 50.000000 (Min: 50, Max: 50)
	#northWestPlayer : 50.000000 (Min: 50, Max: 50)
	#northPlayer : 50.000000 (Min: 50, Max: 50)
	#northEastPlayer : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.00696932s
#Calculated fast perft for depth 4 in 0.246437s
#There are 336969 leaves
#Number of traveled states: 351541 (1426496.950885 states/sec)

	#Connect4
#Preprocessing took: 0.00302647s
#Calculated 100000 games in 3.105586s
#Time for one game: 0.000031s (32200.045850 games/sec)
#Avarage number of turns in game: 21.324400
#Avarage number of moves in one state_: 6.537100
#Number of traveled player states: 2132440 (686646.657730 states/sec)
#Number of traveled keeper states: 2231898 (718672.179332 states/sec)
#Number of traveled states: 4364338 (1405318.837062 states/sec)
#Number of calculated moves: 14408733 (4639618.632446 moves/sec)
#Avarage player scores: 
	#yellow : 44.641500 (Min: 0, Max: 100)
	#red : 55.358500 (Min: 0, Max: 100)
#Preprocessing took: 0.00424371s
#Calculated fast perft for depth 8 in 4.446226s
#There are 5628804 leaves
#Number of traveled states: 6576565 (1479134.263286 states/sec)

	#DoubleChess
#Preprocessing took: 0.163625s
#Calculated 100 games in 22.651821s
#Time for one game: 0.226518s (4.414656 games/sec)
#Avarage number of turns in game: 371.050000
#Avarage number of moves in one state_: 93.140000
#Number of traveled player states: 37105 (1638.058143 states/sec)
#Number of traveled keeper states: 74245 (3277.661416 states/sec)
#Number of traveled states: 111350 (4915.719559 states/sec)
#Number of calculated moves: 3463185 (152887.707606 moves/sec)
#Avarage player scores: 
	#white : 47.000000 (Min: 0, Max: 100)
	#black : 53.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.159517s
#Calculated fast perft for depth 3 in 3.732815s
#There are 389450 leaves
#Number of traveled states: 394707 (105739.756155 states/sec)

	#EnglishCheckers
#Preprocessing took: 0.0143951s
#Calculated 10000 games in 3.940895s
#Time for one game: 0.000394s (2537.494551 games/sec)
#Avarage number of turns in game: 67.789300
#Avarage number of moves in one state_: 4.613300
#Number of traveled player states: 677893 (172014.979372 states/sec)
#Number of traveled keeper states: 1338570 (339661.408126 states/sec)
#Number of traveled states: 2016463 (511676.387498 states/sec)
#Number of calculated moves: 3497279 (887432.640615 moves/sec)
#Avarage player scores: 
	#white : 50.420000 (Min: 0, Max: 100)
	#black : 49.580000 (Min: 0, Max: 100)
#Preprocessing took: 0.014133s
#Calculated fast perft for depth 8 in 1.934661s
#There are 845931 leaves
#Number of traveled states: 1071628 (553910.069139 states/sec)

	#EnglishCheckers-split
#Preprocessing took: 0.0122658s
#Calculated 10000 games in 4.529658s
#Time for one game: 0.000453s (2207.672053 games/sec)
#Avarage number of turns in game: 70.046500
#Avarage number of moves in one state_: 4.458100
#Number of traveled player states: 700465 (154639.700445 states/sec)
#Number of traveled keeper states: 1013377 (223720.408182 states/sec)
#Number of traveled states: 1713842 (378360.108627 states/sec)
#Number of calculated moves: 3524188 (778025.135633 moves/sec)
#Avarage player scores: 
	#white : 51.615000 (Min: 0, Max: 100)
	#black : 48.385000 (Min: 0, Max: 100)
#Preprocessing took: 0.0161167s
#Calculated fast perft for depth 8 in 1.911809s
#There are 838248 leaves
#Number of traveled states: 1063460 (556258.410992 states/sec)

	#Go
#Preprocessing took: 0.0125343s
#Calculated 200 games in 7.707927s
#Time for one game: 0.038540s (25.947313 games/sec)
#Avarage number of turns in game: 474.335000
#Avarage number of moves in one state_: 148.615000
#Number of traveled player states: 94867 (12307.718699 states/sec)
#Number of traveled keeper states: 784098 (101726.181036 states/sec)
#Number of traveled states: 878965 (114033.899735 states/sec)
#Number of calculated moves: 13922430 (1806248.242752 moves/sec)
#Avarage player scores: 
	#white : 180.230000 (Min: 72, Max: 299)
	#black : 173.935000 (Min: 56, Max: 284)
#Preprocessing took: 0.00879864s
#Calculated fast perft for depth 2 in 1.257635s
#There are 130682 leaves
#Number of traveled states: 131045 (104199.566470 states/sec)

	#Gomoku
#Preprocessing took: 0.0070811s
#Calculated 10000 games in 8.954560s
#Time for one game: 0.000895s (1116.749411 games/sec)
#Avarage number of turns in game: 118.450900
#Avarage number of moves in one state_: 166.025200
#Number of traveled player states: 1184509 (132279.972763 states/sec)
#Number of traveled keeper states: 1194509 (133396.722174 states/sec)
#Number of traveled states: 2379018 (265676.694937 states/sec)
#Number of calculated moves: 193096729 (21564065.830503 moves/sec)
#Avarage player scores: 
	#white : 50.130000 (Min: 0, Max: 100)
	#black : 49.870000 (Min: 0, Max: 100)
#Preprocessing took: 0.00541999s
#Calculated fast perft for depth 3 in 7.432613s
#There are 11239200 leaves
#Number of traveled states: 11289826 (1518957.941476 states/sec)

	#Hex
#Preprocessing took: 0.00322679s
#Calculated 10000 games in 4.443665s
#Time for one game: 0.000444s (2250.394885 games/sec)
#Avarage number of turns in game: 107.490900
#Avarage number of moves in one state_: 67.514600
#Number of traveled player states: 1074909 (241896.971520 states/sec)
#Number of traveled keeper states: 1084909 (244147.366405 states/sec)
#Number of traveled states: 2159818 (486044.337925 states/sec)
#Number of calculated moves: 72256109 (16260477.808766 moves/sec)
#Avarage player scores: 
	#red : 52.010000 (Min: 0, Max: 100)
	#blue : 47.990000 (Min: 0, Max: 100)
#Preprocessing took: 0.0033283s
#Calculated fast perft for depth 3 in 0.471693s
#There are 1727880 leaves
#Number of traveled states: 1742522 (3694185.330090 states/sec)

	#InternationalCheckers
#Preprocessing took: 0.018128s
#Calculated 1000 games in 5.039156s
#Time for one game: 0.005039s (198.445913 games/sec)
#Avarage number of turns in game: 93.638000
#Avarage number of moves in one state_: 7.493000
#Number of traveled player states: 93638 (18582.078440 states/sec)
#Number of traveled keeper states: 277935 (55155.064945 states/sec)
#Number of traveled states: 371573 (73737.143386 states/sec)
#Number of calculated moves: 745997 (148040.056070 moves/sec)
#Avarage player scores: 
	#white : 48.350000 (Min: 0, Max: 100)
	#black : 51.650000 (Min: 0, Max: 100)
#Preprocessing took: 0.0182065s
#Calculated fast perft for depth 6 in 2.410945s
#There are 167140 leaves
#Number of traveled states: 199271 (82652.663254 states/sec)

	#PaperSoccer
#Preprocessing took: 0.0189509s
#Calculated 100 games in 3.193295s
#Time for one game: 0.031933s (31.315617 games/sec)
#Avarage number of turns in game: 58.450000
#Avarage number of moves in one state_: 140.180000
#Number of traveled player states: 5845 (1830.397829 states/sec)
#Number of traveled keeper states: 5745 (1799.082212 states/sec)
#Number of traveled states: 11590 (3629.480041 states/sec)
#Number of calculated moves: 825052 (258370.126571 moves/sec)
#Avarage player scores: 
	#white : 45.000000 (Min: 0, Max: 100)
	#black : 55.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0179923s
#Calculated fast perft for depth 5 in 0.184257s
#There are 82026 leaves
#Number of traveled states: 88081 (478034.447136 states/sec)

	#Reversi
#Preprocessing took: 0.0136909s
#Calculated 10000 games in 9.172073s
#Time for one game: 0.000917s (1090.266085 games/sec)
#Avarage number of turns in game: 60.414200
#Avarage number of moves in one state_: 7.982800
#Number of traveled player states: 604142 (65867.553291 states/sec)
#Number of traveled keeper states: 624142 (68048.085460 states/sec)
#Number of traveled states: 1228284 (133915.638751 states/sec)
#Number of calculated moves: 5120300 (558248.943319 moves/sec)
#Avarage player scores: 
	#white : 51.900000 (Min: 0, Max: 100)
	#black : 48.100000 (Min: 0, Max: 100)
#Preprocessing took: 0.01448s
#Calculated fast perft for depth 8 in 2.640652s
#There are 390216 leaves
#Number of traveled states: 455221 (172389.617975 states/sec)

	#TheMillGame
#Preprocessing took: 0.0144822s
#Calculated 10000 games in 12.007779s
#Time for one game: 0.001201s (832.793467 games/sec)
#Avarage number of turns in game: 51.000000
#Avarage number of moves in one state_: 20.781100
#Number of traveled player states: 510000 (42472.466799 states/sec)
#Number of traveled keeper states: 1093982 (91106.106222 states/sec)
#Number of traveled states: 1603982 (133578.573021 states/sec)
#Number of calculated moves: 10847476 (903370.714234 moves/sec)
#Avarage player scores: 
	#white : 50.000000 (Min: 50, Max: 50)
	#black : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0119603s
#Calculated fast perft for depth 5 in 5.848933s
#There are 5140800 leaves
#Number of traveled states: 5408545 (924706.246543 states/sec)

	#TicTacToe
#Preprocessing took: 0.00194866s
#Calculated 1000000 games in 6.623646s
#Time for one game: 0.000007s (150974.261359 games/sec)
#Avarage number of turns in game: 7.753578
#Avarage number of moves in one state_: 5.415552
#Number of traveled player states: 7753578 (1170590.711438 states/sec)
#Number of traveled keeper states: 8499784 (1283248.611110 states/sec)
#Number of traveled states: 16253362 (2453839.322548 states/sec)
#Number of calculated moves: 42527080 (6420494.490748 moves/sec)
#Avarage player scores: 
	#xplayer : 64.813050 (Min: 0, Max: 100)
	#oplayer : 35.186950 (Min: 0, Max: 100)
#Preprocessing took: 0.00290316s
#Calculated fast perft for depth 10 in 0.318503s
#There are 0 leaves
#Number of traveled states: 340858 (1070186.898724 states/sec)
