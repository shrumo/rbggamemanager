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
#Preprocessing took: 0.00312089s
#Calculated 2000 games in 14.469507s
#Time for one game: 0.007235s (138.221710 games/sec)
#Avarage number of turns in game: 72.354000
#Avarage number of moves in one state_: 452.848000
#Number of traveled player states: 144708 (10000.893614 states/sec)
#Number of traveled keeper states: 0 (0.000000 states/sec)
#Number of traveled states: 144708 (10000.893614 states/sec)
#Number of calculated moves: 65243172 (4509011.403923 moves/sec)
#Avarage player scores: 
	#white : 51.800000 (Min: 0, Max: 100)
	#black : 48.200000 (Min: 0, Max: 100)
#Preprocessing took: 0.00300828s
#Calculated perft for depth 2 in 2.927244s
#There are 4307152 leaves
#Number of traveled states: 4309329 (1472145.690250 states/sec)

	#Breakthrough
#Preprocessing took: 0.00181324s
#Calculated 100000 games in 25.872002s
#Time for one game: 0.000259s (3865.182091 games/sec)
#Avarage number of turns in game: 63.999340
#Avarage number of moves in one state_: 25.348570
#Number of traveled player states: 6399934 (247369.102817 states/sec)
#Number of traveled keeper states: 6499934 (251234.284908 states/sec)
#Number of traveled states: 12899868 (498603.387725 states/sec)
#Number of calculated moves: 164493591 (6357976.820515 moves/sec)
#Avarage player scores: 
	#white : 51.078000 (Min: 0, Max: 100)
	#black : 48.922000 (Min: 0, Max: 100)
#Preprocessing took: 0.00180125s
#Calculated perft for depth 5 in 6.415890s
#There are 6182818 leaves
#Number of traveled states: 6450493 (1005393.377176 states/sec)

	#Chess
#Preprocessing took: 0.103143s
#Calculated 1000 games in 18.710699s
#Time for one game: 0.018711s (53.445357 games/sec)
#Avarage number of turns in game: 191.013000
#Avarage number of moves in one state_: 27.346000
#Number of traveled player states: 191013 (10208.757996 states/sec)
#Number of traveled keeper states: 382580 (20447.124719 states/sec)
#Number of traveled states: 573593 (30655.882715 states/sec)
#Number of calculated moves: 5301142 (283321.427224 moves/sec)
#Avarage player scores: 
	#white : 50.700000 (Min: 0, Max: 100)
	#black : 49.300000 (Min: 0, Max: 100)
#Preprocessing took: 0.103632s
#Calculated perft for depth 5 in 31.997904s
#There are 4865609 leaves
#Number of traveled states: 5072213 (158517.038487 states/sec)

	#Chess-KingCapture
#Preprocessing took: 0.0197488s
#Calculated 10000 games in 19.017015s
#Time for one game: 0.001902s (525.844893 games/sec)
#Avarage number of turns in game: 109.228300
#Avarage number of moves in one state_: 32.883200
#Number of traveled player states: 1092283 (57437.143749 states/sec)
#Number of traveled keeper states: 1101110 (57901.307037 states/sec)
#Number of traveled states: 2193393 (115338.450786 states/sec)
#Number of calculated moves: 36849545 (1937714.505548 moves/sec)
#Avarage player scores: 
	#white : 50.045000 (Min: 0, Max: 100)
	#black : 49.955000 (Min: 0, Max: 100)
#Preprocessing took: 0.0215515s
#Calculated perft for depth 5 in 7.885856s
#There are 4896795 leaves
#Number of traveled states: 5103860 (647217.026267 states/sec)

	#ChineseCheckers6
#Preprocessing took: 0.00733187s
#Calculated 1000 games in 5.759185s
#Time for one game: 0.005759s (173.635685 games/sec)
#Avarage number of turns in game: 201.000000
#Avarage number of moves in one state_: 53.396000
#Number of traveled player states: 201000 (34900.772653 states/sec)
#Number of traveled keeper states: 201000 (34900.772653 states/sec)
#Number of traveled states: 402000 (69801.545306 states/sec)
#Number of calculated moves: 10830822 (1880617.195362 moves/sec)
#Avarage player scores: 
	#southWestPlayer : 50.000000 (Min: 50, Max: 50)
	#southPlayer : 50.000000 (Min: 50, Max: 50)
	#southEastPlayer : 50.000000 (Min: 50, Max: 50)
	#northWestPlayer : 50.000000 (Min: 50, Max: 50)
	#northPlayer : 50.000000 (Min: 50, Max: 50)
	#northEastPlayer : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.00804817s
#Calculated perft for depth 4 in 0.484403s
#There are 336969 leaves
#Number of traveled states: 351541 (725719.451019 states/sec)

	#Connect4
#Preprocessing took: 0.00442711s
#Calculated 100000 games in 3.963324s
#Time for one game: 0.000040s (25231.348352 games/sec)
#Avarage number of turns in game: 21.324400
#Avarage number of moves in one state_: 6.537100
#Number of traveled player states: 2132440 (538043.364808 states/sec)
#Number of traveled keeper states: 2231898 (563137.959252 states/sec)
#Number of traveled states: 4364338 (1101181.324060 states/sec)
#Number of calculated moves: 14408733 (3635517.616408 moves/sec)
#Avarage player scores: 
	#yellow : 44.641500 (Min: 0, Max: 100)
	#red : 55.358500 (Min: 0, Max: 100)
#Preprocessing took: 0.00309569s
#Calculated perft for depth 8 in 10.562720s
#There are 5628804 leaves
#Number of traveled states: 6576565 (622620.424693 states/sec)

	#DoubleChess
#Preprocessing took: 0.0233161s
#Calculated 100 games in 28.257756s
#Time for one game: 0.282578s (3.538851 games/sec)
#Avarage number of turns in game: 195.320000
#Avarage number of moves in one state_: 98.170000
#Number of traveled player states: 19532 (691.208455 states/sec)
#Number of traveled keeper states: 39048 (1381.850693 states/sec)
#Number of traveled states: 58580 (2073.059148 states/sec)
#Number of calculated moves: 1930415 (68314.518195 moves/sec)
#Avarage player scores: 
	#white : 54.000000 (Min: 0, Max: 100)
	#black : 46.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0227142s
#Calculated perft for depth 3 in 10.135063s
#There are 389450 leaves
#Number of traveled states: 394707 (38944.702612 states/sec)

	#EnglishCheckers
#Preprocessing took: 0.0160284s
#Calculated 10000 games in 5.962186s
#Time for one game: 0.000596s (1677.237208 games/sec)
#Avarage number of turns in game: 67.450800
#Avarage number of moves in one state_: 4.612400
#Number of traveled player states: 674508 (113130.991481 states/sec)
#Number of traveled keeper states: 1331648 (223348.957379 states/sec)
#Number of traveled states: 2006156 (336479.948861 states/sec)
#Number of calculated moves: 3479187 (583542.189060 moves/sec)
#Avarage player scores: 
	#white : 50.600000 (Min: 0, Max: 100)
	#black : 49.400000 (Min: 0, Max: 100)
#Preprocessing took: 0.0159518s
#Calculated perft for depth 8 in 3.899510s
#There are 845931 leaves
#Number of traveled states: 1071628 (274810.943493 states/sec)

	#EnglishCheckers-split
#Preprocessing took: 0.016831s
#Calculated 10000 games in 5.902190s
#Time for one game: 0.000590s (1694.286216 games/sec)
#Avarage number of turns in game: 70.046500
#Avarage number of moves in one state_: 4.458100
#Number of traveled player states: 700465 (118678.819430 states/sec)
#Number of traveled keeper states: 1361073 (230604.722290 states/sec)
#Number of traveled states: 2061538 (349283.541720 states/sec)
#Number of calculated moves: 3524188 (597098.315106 moves/sec)
#Avarage player scores: 
	#white : 51.615000 (Min: 0, Max: 100)
	#black : 48.385000 (Min: 0, Max: 100)
#Preprocessing took: 0.0178243s
#Calculated perft for depth 8 in 3.717803s
#There are 838248 leaves
#Number of traveled states: 1063460 (286045.300741 states/sec)

	#Go
#Preprocessing took: 0.0105028s
#Calculated 200 games in 7.046658s
#Time for one game: 0.035233s (28.382248 games/sec)
#Avarage number of turns in game: 474.335000
#Avarage number of moves in one state_: 148.615000
#Number of traveled player states: 94867 (13462.693519 states/sec)
#Number of traveled keeper states: 784098 (111272.318747 states/sec)
#Number of traveled states: 878965 (124735.012265 states/sec)
#Number of calculated moves: 13922430 (1975749.292421 moves/sec)
#Avarage player scores: 
	#white : 180.230000 (Min: 72, Max: 299)
	#black : 173.935000 (Min: 56, Max: 284)
#Preprocessing took: 0.01214s
#Calculated perft for depth 2 in 1.444997s
#There are 130682 leaves
#Number of traveled states: 131045 (90688.751019 states/sec)

	#Gomoku
#Preprocessing took: 0.00522817s
#Calculated 10000 games in 8.881136s
#Time for one game: 0.000888s (1125.982029 games/sec)
#Avarage number of turns in game: 118.450900
#Avarage number of moves in one state_: 166.025200
#Number of traveled player states: 1184509 (133373.584744 states/sec)
#Number of traveled keeper states: 1194509 (134499.566773 states/sec)
#Number of traveled states: 2379018 (267873.151517 states/sec)
#Number of calculated moves: 193096729 (21742344.675322 moves/sec)
#Avarage player scores: 
	#white : 50.130000 (Min: 0, Max: 100)
	#black : 49.870000 (Min: 0, Max: 100)
#Preprocessing took: 0.00531225s
#Calculated perft for depth 3 in 12.506324s
#There are 11239200 leaves
#Number of traveled states: 11289826 (902729.344666 states/sec)

	#Hex
#Preprocessing took: 0.00389126s
#Calculated 10000 games in 4.768879s
#Time for one game: 0.000477s (2096.928808 games/sec)
#Avarage number of turns in game: 107.490900
#Avarage number of moves in one state_: 67.514600
#Number of traveled player states: 1074909 (225400.764756 states/sec)
#Number of traveled keeper states: 1084909 (227497.693563 states/sec)
#Number of traveled states: 2159818 (452898.458319 states/sec)
#Number of calculated moves: 72256109 (15151591.648113 moves/sec)
#Avarage player scores: 
	#red : 52.010000 (Min: 0, Max: 100)
	#blue : 47.990000 (Min: 0, Max: 100)
#Preprocessing took: 0.00326599s
#Calculated perft for depth 3 in 1.125852s
#There are 1727880 leaves
#Number of traveled states: 1742522 (1547736.942404 states/sec)

	#InternationalCheckers
#Preprocessing took: 0.0189277s
#Calculated 1000 games in 7.050756s
#Time for one game: 0.007051s (141.828771 games/sec)
#Avarage number of turns in game: 93.701000
#Avarage number of moves in one state_: 7.472000
#Number of traveled player states: 93701 (13289.497627 states/sec)
#Number of traveled keeper states: 278127 (39446.410460 states/sec)
#Number of traveled states: 371828 (52735.908086 states/sec)
#Number of calculated moves: 745188 (105689.097849 moves/sec)
#Avarage player scores: 
	#white : 46.100000 (Min: 0, Max: 100)
	#black : 53.900000 (Min: 0, Max: 100)
#Preprocessing took: 0.0173674s
#Calculated perft for depth 6 in 3.307951s
#There are 166992 leaves
#Number of traveled states: 199123 (60195.271353 states/sec)

	#PaperSoccer
#Preprocessing took: 0.0198069s
#Calculated 100 games in 3.205908s
#Time for one game: 0.032059s (31.192410 games/sec)
#Avarage number of turns in game: 58.450000
#Avarage number of moves in one state_: 140.180000
#Number of traveled player states: 5845 (1823.196392 states/sec)
#Number of traveled keeper states: 5745 (1792.003981 states/sec)
#Number of traveled states: 11590 (3615.200373 states/sec)
#Number of calculated moves: 825052 (257353.606400 moves/sec)
#Avarage player scores: 
	#white : 45.000000 (Min: 0, Max: 100)
	#black : 55.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0193412s
#Calculated perft for depth 5 in 0.244861s
#There are 82026 leaves
#Number of traveled states: 88081 (359718.760135 states/sec)

	#Reversi
#Preprocessing took: 0.0104958s
#Calculated 10000 games in 10.525515s
#Time for one game: 0.001053s (950.072277 games/sec)
#Avarage number of turns in game: 61.420600
#Avarage number of moves in one state_: 7.877000
#Number of traveled player states: 614206 (58354.009270 states/sec)
#Number of traveled keeper states: 628715 (59732.469136 states/sec)
#Number of traveled states: 1242921 (118086.478406 states/sec)
#Number of calculated moves: 5139317 (488272.260219 moves/sec)
#Avarage player scores: 
	#white : 48.350000 (Min: 0, Max: 100)
	#black : 51.650000 (Min: 0, Max: 100)
#Preprocessing took: 0.00926478s
#Calculated perft for depth 8 in 3.345905s
#There are 431044 leaves
#Number of traveled states: 500877 (149698.521860 states/sec)

	#TheMillGame
#Preprocessing took: 0.0112108s
#Calculated 10000 games in 13.270694s
#Time for one game: 0.001327s (753.540078 games/sec)
#Avarage number of turns in game: 51.000000
#Avarage number of moves in one state_: 20.781100
#Number of traveled player states: 510000 (38430.543993 states/sec)
#Number of traveled keeper states: 1093982 (82435.928193 states/sec)
#Number of traveled states: 1603982 (120866.472186 states/sec)
#Number of calculated moves: 10847476 (817400.791434 moves/sec)
#Avarage player scores: 
	#white : 50.000000 (Min: 50, Max: 50)
	#black : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0145508s
#Calculated perft for depth 5 in 12.282836s
#There are 5140800 leaves
#Number of traveled states: 5408545 (440333.571142 states/sec)

	#TicTacToe
#Preprocessing took: 0.00287545s
#Calculated 1000000 games in 6.854555s
#Time for one game: 0.000007s (145888.400241 games/sec)
#Avarage number of turns in game: 7.753578
#Avarage number of moves in one state_: 5.415552
#Number of traveled player states: 7753578 (1131157.090566 states/sec)
#Number of traveled keeper states: 8499784 (1240019.890156 states/sec)
#Number of traveled states: 16253362 (2371176.980722 states/sec)
#Number of calculated moves: 42527080 (6204207.668131 moves/sec)
#Avarage player scores: 
	#xplayer : 64.813050 (Min: 0, Max: 100)
	#oplayer : 35.186950 (Min: 0, Max: 100)
#Preprocessing took: 0.00198884s
#Calculated perft for depth 10 in 0.633416s
#There are 0 leaves
#Number of traveled states: 340858 (538126.956690 states/sec)
