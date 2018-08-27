#/bin/bash

cd "${0%/*}"
./make.sh
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

	#Amazons
#Preprocessing took: 0.0215453s
#Calculated 2000 games in 7.930945s
#Time for one game: 0.003965s (252.176771 games/sec)
#Avarage number of turns in game: 72.354000
#Avarage number of moves in one state_: 452.848000
#Number of traveled player states: 144708 (18245.998076 states/sec)
#Number of traveled keeper states: 142708 (17993.821305 states/sec)
#Number of traveled states: 287416 (36239.819380 states/sec)
#Number of calculated moves: 65243172 (8226406.216337 moves/sec)
#Avarage player scores: 
	#white : 51.800000 (Min: 0, Max: 100)
	#black : 48.200000 (Min: 0, Max: 100)
#Preprocessing took: 0.00805393s
#Calculated perft for depth 2 in 1.058516s
#There are 4307152 leaves
#Number of traveled states: 4309329 (4071102.397616 states/sec)

	#Breakthrough
#Preprocessing took: 0.00255638s
#Calculated 100000 games in 25.530019s
#Time for one game: 0.000255s (3916.957492 games/sec)
#Avarage number of turns in game: 63.999340
#Avarage number of moves in one state_: 25.348570
#Number of traveled player states: 6399934 (250682.694306 states/sec)
#Number of traveled keeper states: 6499934 (254599.651798 states/sec)
#Number of traveled states: 12899868 (505282.346105 states/sec)
#Number of calculated moves: 164493591 (6443144.036797 moves/sec)
#Avarage player scores: 
	#white : 51.078000 (Min: 0, Max: 100)
	#black : 48.922000 (Min: 0, Max: 100)
#Preprocessing took: 0.00258327s
#Calculated perft for depth 5 in 2.169220s
#There are 6182818 leaves
#Number of traveled states: 6450493 (2973646.931899 states/sec)

	#Chess
#Preprocessing took: 0.100334s
#Calculated 1000 games in 16.298108s
#Time for one game: 0.016298s (61.356814 games/sec)
#Avarage number of turns in game: 191.013000
#Avarage number of moves in one state_: 27.346000
#Number of traveled player states: 191013 (11719.949051 states/sec)
#Number of traveled keeper states: 382580 (23473.889776 states/sec)
#Number of traveled states: 573593 (35193.838827 states/sec)
#Number of calculated moves: 5301142 (325261.181963 moves/sec)
#Avarage player scores: 
	#white : 50.700000 (Min: 0, Max: 100)
	#black : 49.300000 (Min: 0, Max: 100)
#Preprocessing took: 0.102609s
#Calculated perft for depth 5 in 24.473900s
#There are 4865609 leaves
#Number of traveled states: 5072213 (207249.886834 states/sec)

	#Chess-KingCapture
#Preprocessing took: 0.0284478s
#Calculated 10000 games in 16.981298s
#Time for one game: 0.001698s (588.883145 games/sec)
#Avarage number of turns in game: 109.228300
#Avarage number of moves in one state_: 32.883200
#Number of traveled player states: 1092283 (64322.704851 states/sec)
#Number of traveled keeper states: 1101110 (64842.512003 states/sec)
#Number of traveled states: 2193393 (129165.216853 states/sec)
#Number of calculated moves: 36849545 (2170007.595935 moves/sec)
#Avarage player scores: 
	#white : 50.045000 (Min: 0, Max: 100)
	#black : 49.955000 (Min: 0, Max: 100)
#Preprocessing took: 0.0168025s
#Calculated perft for depth 5 in 4.391645s
#There are 4896795 leaves
#Number of traveled states: 5104321 (1162279.949828 states/sec)

	#ChineseCheckers6
#Preprocessing took: 0.0307518s
#Calculated 1000 games in 5.039193s
#Time for one game: 0.005039s (198.444456 games/sec)
#Avarage number of turns in game: 201.000000
#Avarage number of moves in one state_: 53.396000
#Number of traveled player states: 201000 (39887.335613 states/sec)
#Number of traveled keeper states: 201000 (39887.335613 states/sec)
#Number of traveled states: 402000 (79774.671226 states/sec)
#Number of calculated moves: 10830822 (2149316.577506 moves/sec)
#Avarage player scores: 
	#southWestPlayer : 50.000000 (Min: 50, Max: 50)
	#southPlayer : 50.000000 (Min: 50, Max: 50)
	#southEastPlayer : 50.000000 (Min: 50, Max: 50)
	#northWestPlayer : 50.000000 (Min: 50, Max: 50)
	#northPlayer : 50.000000 (Min: 50, Max: 50)
	#northEastPlayer : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0137418s
#Calculated perft for depth 4 in 0.261440s
#There are 336969 leaves
#Number of traveled states: 351541 (1344633.264484 states/sec)

	#Connect4
#Preprocessing took: 0.00305813s
#Calculated 100000 games in 3.048728s
#Time for one game: 0.000030s (32800.564447 games/sec)
#Avarage number of turns in game: 21.324400
#Avarage number of moves in one state_: 6.537100
#Number of traveled player states: 2132440 (699452.356495 states/sec)
#Number of traveled keeper states: 2231898 (732075.141883 states/sec)
#Number of traveled states: 4364338 (1431527.498378 states/sec)
#Number of calculated moves: 14408733 (4726145.753671 moves/sec)
#Avarage player scores: 
	#yellow : 44.641500 (Min: 0, Max: 100)
	#red : 55.358500 (Min: 0, Max: 100)
#Preprocessing took: 0.00457828s
#Calculated perft for depth 8 in 4.721507s
#There are 5628804 leaves
#Number of traveled states: 6634027 (1405065.701813 states/sec)

	#DoubleChess
#Preprocessing took: 0.0318778s
#Calculated 100 games in 23.762921s
#Time for one game: 0.237629s (4.208237 games/sec)
#Avarage number of turns in game: 195.320000
#Avarage number of moves in one state_: 98.170000
#Number of traveled player states: 19532 (821.952826 states/sec)
#Number of traveled keeper states: 39048 (1643.232334 states/sec)
#Number of traveled states: 58580 (2465.185159 states/sec)
#Number of calculated moves: 1930415 (81236.435804 moves/sec)
#Avarage player scores: 
	#white : 54.000000 (Min: 0, Max: 100)
	#black : 46.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0242477s
#Calculated perft for depth 3 in 8.603443s
#There are 389450 leaves
#Number of traveled states: 394707 (45877.795322 states/sec)

	#EnglishCheckers
#Preprocessing took: 0.0343762s
#Calculated 10000 games in 6.128308s
#Time for one game: 0.000613s (1631.771730 games/sec)
#Avarage number of turns in game: 67.450800
#Avarage number of moves in one state_: 4.612400
#Number of traveled player states: 674508 (110064.308623 states/sec)
#Number of traveled keeper states: 1331648 (217294.556105 states/sec)
#Number of traveled states: 2006156 (327358.864729 states/sec)
#Number of calculated moves: 3479187 (567723.899088 moves/sec)
#Avarage player scores: 
	#white : 50.600000 (Min: 0, Max: 100)
	#black : 49.400000 (Min: 0, Max: 100)
#Preprocessing took: 0.0167917s
#Calculated perft for depth 8 in 2.805578s
#There are 845931 leaves
#Number of traveled states: 1071628 (381963.358365 states/sec)

	#EnglishCheckers-split
#Preprocessing took: 0.0212012s
#Calculated 10000 games in 6.083919s
#Time for one game: 0.000608s (1643.677295 games/sec)
#Avarage number of turns in game: 70.046500
#Avarage number of moves in one state_: 4.458100
#Number of traveled player states: 700465 (115133.841645 states/sec)
#Number of traveled keeper states: 1361073 (223716.478695 states/sec)
#Number of traveled states: 2061538 (338850.320340 states/sec)
#Number of calculated moves: 3524188 (579262.779895 moves/sec)
#Avarage player scores: 
	#white : 51.615000 (Min: 0, Max: 100)
	#black : 48.385000 (Min: 0, Max: 100)
#Preprocessing took: 0.019544s
#Calculated perft for depth 8 in 2.731421s
#There are 838248 leaves
#Number of traveled states: 1063460 (389343.062033 states/sec)

	#Go
#Preprocessing took: 0.0232027s
#Calculated 200 games in 7.897410s
#Time for one game: 0.039487s (25.324759 games/sec)
#Avarage number of turns in game: 474.335000
#Avarage number of moves in one state_: 148.615000
#Number of traveled player states: 94867 (12012.419619 states/sec)
#Number of traveled keeper states: 784098 (99285.464897 states/sec)
#Number of traveled states: 878965 (111297.884516 states/sec)
#Number of calculated moves: 13922430 (1762910.930835 moves/sec)
#Avarage player scores: 
	#white : 180.230000 (Min: 72, Max: 299)
	#black : 173.935000 (Min: 56, Max: 284)
#Preprocessing took: 0.0134499s
#Calculated perft for depth 2 in 1.345902s
#There are 130682 leaves
#Number of traveled states: 131046 (97366.709156 states/sec)

	#Gomoku
#Preprocessing took: 0.0319848s
#Calculated 10000 games in 9.052752s
#Time for one game: 0.000905s (1104.636491 games/sec)
#Avarage number of turns in game: 118.450900
#Avarage number of moves in one state_: 166.025200
#Number of traveled player states: 1184509 (130845.186475 states/sec)
#Number of traveled keeper states: 1194509 (131949.822966 states/sec)
#Number of traveled states: 2379018 (262795.009441 states/sec)
#Number of calculated moves: 193096729 (21330169.305359 moves/sec)
#Avarage player scores: 
	#white : 50.130000 (Min: 0, Max: 100)
	#black : 49.870000 (Min: 0, Max: 100)
#Preprocessing took: 0.0110103s
#Calculated perft for depth 3 in 7.455508s
#There are 11239200 leaves
#Number of traveled states: 11289826 (1514293.399082 states/sec)

	#Hex
#Preprocessing took: 0.0196873s
#Calculated 10000 games in 4.517931s
#Time for one game: 0.000452s (2213.402581 games/sec)
#Avarage number of turns in game: 107.490900
#Avarage number of moves in one state_: 67.514600
#Number of traveled player states: 1074909 (237920.635519 states/sec)
#Number of traveled keeper states: 1084909 (240134.038100 states/sec)
#Number of traveled states: 2159818 (478054.673619 states/sec)
#Number of calculated moves: 72256109 (15993185.817043 moves/sec)
#Avarage player scores: 
	#red : 52.010000 (Min: 0, Max: 100)
	#blue : 47.990000 (Min: 0, Max: 100)
#Preprocessing took: 0.00401794s
#Calculated perft for depth 3 in 0.508722s
#There are 1727880 leaves
#Number of traveled states: 1742522 (3425292.539288 states/sec)

	#InternationalCheckers
#Preprocessing took: 0.0328251s
#Calculated 1000 games in 7.365489s
#Time for one game: 0.007365s (135.768316 games/sec)
#Avarage number of turns in game: 93.701000
#Avarage number of moves in one state_: 7.472000
#Number of traveled player states: 93701 (12721.627013 states/sec)
#Number of traveled keeper states: 278127 (37760.834531 states/sec)
#Number of traveled states: 371828 (50482.461544 states/sec)
#Number of calculated moves: 745188 (101172.920149 moves/sec)
#Avarage player scores: 
	#white : 46.100000 (Min: 0, Max: 100)
	#black : 53.900000 (Min: 0, Max: 100)
#Preprocessing took: 0.016568s
#Calculated perft for depth 6 in 3.281079s
#There are 167140 leaves
#Number of traveled states: 199271 (60733.381375 states/sec)

	#PaperSoccer
#Preprocessing took: 0.0382507s
#Calculated 100 games in 3.196058s
#Time for one game: 0.031961s (31.288539 games/sec)
#Avarage number of turns in game: 58.450000
#Avarage number of moves in one state_: 140.180000
#Number of traveled player states: 5845 (1828.815133 states/sec)
#Number of traveled keeper states: 5745 (1797.526593 states/sec)
#Number of traveled states: 11590 (3626.341726 states/sec)
#Number of calculated moves: 825052 (258146.720771 moves/sec)
#Avarage player scores: 
	#white : 45.000000 (Min: 0, Max: 100)
	#black : 55.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0218644s
#Calculated perft for depth 5 in 0.186641s
#There are 82026 leaves
#Number of traveled states: 88081 (471926.725014 states/sec)

	#Reversi
#Preprocessing took: 0.00970115s
#Calculated 10000 games in 9.882984s
#Time for one game: 0.000988s (1011.840154 games/sec)
#Avarage number of turns in game: 61.420600
#Avarage number of moves in one state_: 7.877000
#Number of traveled player states: 614206 (62147.829382 states/sec)
#Number of traveled keeper states: 628715 (63615.908262 states/sec)
#Number of traveled states: 1242921 (125763.737644 states/sec)
#Number of calculated moves: 5139317 (520016.730635 moves/sec)
#Avarage player scores: 
	#white : 48.350000 (Min: 0, Max: 100)
	#black : 51.650000 (Min: 0, Max: 100)
#Preprocessing took: 0.0108014s
#Calculated perft for depth 8 in 2.764067s
#There are 390216 leaves
#Number of traveled states: 455221 (164692.462826 states/sec)

	#TheMillGame
#Preprocessing took: 0.0161126s
#Calculated 10000 games in 12.088300s
#Time for one game: 0.001209s (827.246173 games/sec)
#Avarage number of turns in game: 51.000000
#Avarage number of moves in one state_: 20.781100
#Number of traveled player states: 510000 (42189.554827 states/sec)
#Number of traveled keeper states: 1093982 (90499.242291 states/sec)
#Number of traveled states: 1603982 (132688.797118 states/sec)
#Number of calculated moves: 10847476 (897353.300851 moves/sec)
#Avarage player scores: 
	#white : 50.000000 (Min: 50, Max: 50)
	#black : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0112521s
#Calculated perft for depth 5 in 5.983100s
#There are 5140800 leaves
#Number of traveled states: 5408545 (903970.382000 states/sec)

	#TicTacToe
#Preprocessing took: 0.0189252s
#Calculated 1000000 games in 6.671029s
#Time for one game: 0.000007s (149901.903752 games/sec)
#Avarage number of turns in game: 7.753578
#Avarage number of moves in one state_: 5.415552
#Number of traveled player states: 7753578 (1162276.103089 states/sec)
#Number of traveled keeper states: 8499784 (1274133.803081 states/sec)
#Number of traveled states: 16253362 (2436409.906170 states/sec)
#Number of calculated moves: 42527080 (6374890.253013 moves/sec)
#Avarage player scores: 
	#xplayer : 64.813050 (Min: 0, Max: 100)
	#oplayer : 35.186950 (Min: 0, Max: 100)
#Preprocessing took: 0.00516713s
#Calculated perft for depth 10 in 0.374160s
#There are 0 leaves
#Number of traveled states: 549946 (1469816.772984 states/sec)
