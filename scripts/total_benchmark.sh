#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tArimaa"
../build/benchmark ../rbgParser/examples/arimaa.rbg -n 2 -s 0
../build/benchmark ../rbgParser/examples/arimaa-fixedPosition.rbg -d 1

echo -e "\n\tAmazons"
../build/benchmark ../rbgParser/examples/amazons.rbg -n 2000 -s 0
../build/benchmark ../rbgParser/examples/amazons.rbg -d 2

echo -e "\n\tAmazons-split"
../build/benchmark ../rbgParser/examples/amazons-split.rbg -n 20000 -s 0
../build/benchmark ../rbgParser/examples/amazons-split.rbg -d 4

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

echo -e "\n\tHex 9x9"
../build/benchmark ../rbgParser/examples/hex_9x9.rbg -n 20000 -s 0
../build/benchmark ../rbgParser/examples/hex_9x9.rbg -d 3

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
#Preprocessing took: 0.133657s
#Calculated 2 games in 231.622642s
#Time for one game: 115.811321s (0.008635 games/sec)
#Avarage number of turns in game: 141.500000
#Avarage number of moves in one state_: 194619.000000
#Number of traveled player states: 283 (1.221815 states/sec)
#Number of traveled keeper states: 2048 (8.841968 states/sec)
#Number of traveled states: 2331 (10.063783 states/sec)
#Number of calculated moves: 55151057 (238107.364835 moves/sec)
#Avarage player scores: 
	#silver : 50.000000 (Min: 0, Max: 100)
	#gold : 50.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.132754s
#Calculated fast perft for depth 1 in 1.404769s
#There are 20652 leaves
#Number of traveled states: 20653 (14702.056406 states/sec)

	#Amazons
#Preprocessing took: 0.00307093s
#Calculated 2000 games in 7.747709s
#Time for one game: 0.003874s (258.140826 games/sec)
#Avarage number of turns in game: 72.354000
#Avarage number of moves in one state_: 452.848000
#Number of traveled player states: 144708 (18677.521290 states/sec)
#Number of traveled keeper states: 142708 (18419.380464 states/sec)
#Number of traveled states: 287416 (37096.901754 states/sec)
#Number of calculated moves: 65243172 (8420963.139861 moves/sec)
#Avarage player scores: 
	#white : 51.800000 (Min: 0, Max: 100)
	#black : 48.200000 (Min: 0, Max: 100)
#Preprocessing took: 0.00302637s
#Calculated fast perft for depth 2 in 0.959145s
#There are 4307152 leaves
#Number of traveled states: 4309329 (4492885.298719 states/sec)

	#Amazons-split
#Preprocessing took: 0.0237867s
#Calculated 20000 games in 12.756407s
#Time for one game: 0.000638s (1567.839550 games/sec)
#Avarage number of turns in game: 134.380700
#Avarage number of moves in one state_: 20.455500
#Number of traveled player states: 2687614 (210687.376200 states/sec)
#Number of traveled keeper states: 1333807 (104559.768325 states/sec)
#Number of traveled states: 4021421 (315247.144525 states/sec)
#Number of calculated moves: 55994801 (4389543.179764 moves/sec)
#Avarage player scores: 
	#white : 49.835000 (Min: 0, Max: 100)
	#black : 50.165000 (Min: 0, Max: 100)
#Preprocessing took: 0.00772005s
#Calculated fast perft for depth 4 in 0.997157s
#There are 4307152 leaves
#Number of traveled states: 4477829 (4490594.502805 states/sec)

	#Breakthrough
#Preprocessing took: 0.0239023s
#Calculated 100000 games in 24.659180s
#Time for one game: 0.000247s (4055.284907 games/sec)
#Avarage number of turns in game: 63.999340
#Avarage number of moves in one state_: 25.348570
#Number of traveled player states: 6399934 (259535.557543 states/sec)
#Number of traveled keeper states: 6499934 (263590.842449 states/sec)
#Number of traveled states: 12899868 (523126.399992 states/sec)
#Number of calculated moves: 164493591 (6670683.768362 moves/sec)
#Avarage player scores: 
	#white : 51.078000 (Min: 0, Max: 100)
	#black : 48.922000 (Min: 0, Max: 100)
#Preprocessing took: 0.00295038s
#Calculated fast perft for depth 5 in 1.854416s
#There are 6182818 leaves
#Number of traveled states: 6450493 (3478449.894273 states/sec)

	#Chess
#Preprocessing took: 0.11221s
#Calculated 1000 games in 16.111900s
#Time for one game: 0.016112s (62.065928 games/sec)
#Avarage number of turns in game: 191.013000
#Avarage number of moves in one state_: 27.346000
#Number of traveled player states: 191013 (11855.399161 states/sec)
#Number of traveled keeper states: 382580 (23745.182847 states/sec)
#Number of traveled states: 573593 (35600.582009 states/sec)
#Number of calculated moves: 5301142 (329020.299256 moves/sec)
#Avarage player scores: 
	#white : 50.700000 (Min: 0, Max: 100)
	#black : 49.300000 (Min: 0, Max: 100)
#Preprocessing took: 0.101153s
#Calculated fast perft for depth 5 in 23.455564s
#There are 4865609 leaves
#Number of traveled states: 5072213 (216247.752882 states/sec)

	#Chess-KingCapture
#Preprocessing took: 0.0302846s
#Calculated 10000 games in 16.225975s
#Time for one game: 0.001623s (616.295787 games/sec)
#Avarage number of turns in game: 109.228300
#Avarage number of moves in one state_: 32.883200
#Number of traveled player states: 1092283 (67316.941146 states/sec)
#Number of traveled keeper states: 1101110 (67860.945437 states/sec)
#Number of traveled states: 2193393 (135177.886583 states/sec)
#Number of calculated moves: 36849545 (2271021.934801 moves/sec)
#Avarage player scores: 
	#white : 50.045000 (Min: 0, Max: 100)
	#black : 49.955000 (Min: 0, Max: 100)
#Preprocessing took: 0.0205313s
#Calculated fast perft for depth 5 in 3.728274s
#There are 4896795 leaves
#Number of traveled states: 5104321 (1369084.319169 states/sec)

	#ChineseCheckers6
#Preprocessing took: 0.0295393s
#Calculated 1000 games in 4.866786s
#Time for one game: 0.004867s (205.474413 games/sec)
#Avarage number of turns in game: 201.000000
#Avarage number of moves in one state_: 53.396000
#Number of traveled player states: 201000 (41300.356986 states/sec)
#Number of traveled keeper states: 201000 (41300.356986 states/sec)
#Number of traveled states: 402000 (82600.713972 states/sec)
#Number of calculated moves: 10830822 (2225456.791297 moves/sec)
#Avarage player scores: 
	#southWestPlayer : 50.000000 (Min: 50, Max: 50)
	#southPlayer : 50.000000 (Min: 50, Max: 50)
	#southEastPlayer : 50.000000 (Min: 50, Max: 50)
	#northWestPlayer : 50.000000 (Min: 50, Max: 50)
	#northPlayer : 50.000000 (Min: 50, Max: 50)
	#northEastPlayer : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0104794s
#Calculated fast perft for depth 4 in 0.235942s
#There are 336969 leaves
#Number of traveled states: 351541 (1489946.770220 states/sec)

	#Connect4
#Preprocessing took: 0.00854294s
#Calculated 100000 games in 3.053596s
#Time for one game: 0.000031s (32748.271952 games/sec)
#Avarage number of turns in game: 21.324400
#Avarage number of moves in one state_: 6.537100
#Number of traveled player states: 2132440 (698337.250407 states/sec)
#Number of traveled keeper states: 2231898 (730908.026725 states/sec)
#Number of traveled states: 4364338 (1429245.277132 states/sec)
#Number of calculated moves: 14408733 (4718611.067636 moves/sec)
#Avarage player scores: 
	#yellow : 44.641500 (Min: 0, Max: 100)
	#red : 55.358500 (Min: 0, Max: 100)
#Preprocessing took: 0.00305265s
#Calculated fast perft for depth 8 in 4.200948s
#There are 5628804 leaves
#Number of traveled states: 6634027 (1579173.703536 states/sec)

	#DoubleChess
#Preprocessing took: 0.174606s
#Calculated 100 games in 22.517758s
#Time for one game: 0.225178s (4.440940 games/sec)
#Avarage number of turns in game: 371.050000
#Avarage number of moves in one state_: 93.140000
#Number of traveled player states: 37105 (1647.810611 states/sec)
#Number of traveled keeper states: 74245 (3297.175551 states/sec)
#Number of traveled states: 111350 (4944.986163 states/sec)
#Number of calculated moves: 3463185 (153797.951540 moves/sec)
#Avarage player scores: 
	#white : 47.000000 (Min: 0, Max: 100)
	#black : 53.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.164315s
#Calculated fast perft for depth 3 in 3.686304s
#There are 389450 leaves
#Number of traveled states: 394707 (107073.916486 states/sec)

	#EnglishCheckers
#Preprocessing took: 0.0210616s
#Calculated 10000 games in 3.920971s
#Time for one game: 0.000392s (2550.388529 games/sec)
#Avarage number of turns in game: 67.789300
#Avarage number of moves in one state_: 4.613300
#Number of traveled player states: 677893 (172889.053125 states/sec)
#Number of traveled keeper states: 1338570 (341387.357358 states/sec)
#Number of traveled states: 2016463 (514276.410483 states/sec)
#Number of calculated moves: 3497279 (891942.024514 moves/sec)
#Avarage player scores: 
	#white : 50.420000 (Min: 0, Max: 100)
	#black : 49.580000 (Min: 0, Max: 100)
#Preprocessing took: 0.0123148s
#Calculated fast perft for depth 8 in 1.838807s
#There are 845931 leaves
#Number of traveled states: 1071628 (582784.257790 states/sec)

	#EnglishCheckers-split
#Preprocessing took: 0.0297809s
#Calculated 10000 games in 4.330534s
#Time for one game: 0.000433s (2309.183862 games/sec)
#Avarage number of turns in game: 70.046500
#Avarage number of moves in one state_: 4.458100
#Number of traveled player states: 700465 (161750.247423 states/sec)
#Number of traveled keeper states: 1013377 (234007.381501 states/sec)
#Number of traveled states: 1713842 (395757.628925 states/sec)
#Number of calculated moves: 3524188 (813799.805796 moves/sec)
#Avarage player scores: 
	#white : 51.615000 (Min: 0, Max: 100)
	#black : 48.385000 (Min: 0, Max: 100)
#Preprocessing took: 0.0131101s
#Calculated fast perft for depth 8 in 1.806841s
#There are 838248 leaves
#Number of traveled states: 1063460 (588574.048404 states/sec)

	#Go
#Preprocessing took: 0.0180925s
#Calculated 200 games in 7.060325s
#Time for one game: 0.035302s (28.327309 games/sec)
#Avarage number of turns in game: 474.335000
#Avarage number of moves in one state_: 148.615000
#Number of traveled player states: 94867 (13436.633984 states/sec)
#Number of traveled keeper states: 784098 (111056.930585 states/sec)
#Number of traveled states: 878965 (124493.564569 states/sec)
#Number of calculated moves: 13922430 (1971924.864093 moves/sec)
#Avarage player scores: 
	#white : 180.230000 (Min: 72, Max: 299)
	#black : 173.935000 (Min: 56, Max: 284)
#Preprocessing took: 0.0108546s
#Calculated fast perft for depth 2 in 1.231841s
#There are 130682 leaves
#Number of traveled states: 131046 (106382.201561 states/sec)

	#Gomoku
#Preprocessing took: 0.0222122s
#Calculated 10000 games in 8.910999s
#Time for one game: 0.000891s (1122.208649 games/sec)
#Avarage number of turns in game: 118.450900
#Avarage number of moves in one state_: 166.025200
#Number of traveled player states: 1184509 (132926.624510 states/sec)
#Number of traveled keeper states: 1194509 (134048.833159 states/sec)
#Number of traveled states: 2379018 (266975.457669 states/sec)
#Number of calculated moves: 193096729 (21669481.945520 moves/sec)
#Avarage player scores: 
	#white : 50.130000 (Min: 0, Max: 100)
	#black : 49.870000 (Min: 0, Max: 100)
#Preprocessing took: 0.00668468s
#Calculated fast perft for depth 3 in 7.072696s
#There are 11239200 leaves
#Number of traveled states: 11289826 (1596255.004414 states/sec)

	#Hex
#Preprocessing took: 0.00361031s
#Calculated 10000 games in 4.432068s
#Time for one game: 0.000443s (2256.283182 games/sec)
#Avarage number of turns in game: 107.490900
#Avarage number of moves in one state_: 67.514600
#Number of traveled player states: 1074909 (242529.909936 states/sec)
#Number of traveled keeper states: 1084909 (244786.193119 states/sec)
#Number of traveled states: 2159818 (487316.103055 states/sec)
#Number of calculated moves: 72256109 (16303024.356589 moves/sec)
#Avarage player scores: 
	#red : 52.010000 (Min: 0, Max: 100)
	#blue : 47.990000 (Min: 0, Max: 100)
#Preprocessing took: 0.00417634s
#Calculated fast perft for depth 3 in 0.475643s
#There are 1727880 leaves
#Number of traveled states: 1742522 (3663506.702690 states/sec)

	#Hex 9x9
#Preprocessing took: 0.0192601s
#Calculated 20000 games in 4.376643s
#Time for one game: 0.000219s (4569.712938 games/sec)
#Avarage number of turns in game: 70.932150
#Avarage number of moves in one state_: 45.801700
#Number of traveled player states: 1418643 (324139.563593 states/sec)
#Number of traveled keeper states: 1438643 (328709.276531 states/sec)
#Number of traveled states: 2857286 (652848.840124 states/sec)
#Number of calculated moves: 64661417 (14774205.693520 moves/sec)
#Avarage player scores: 
	#red : 53.555000 (Min: 0, Max: 100)
	#blue : 46.445000 (Min: 0, Max: 100)
#Preprocessing took: 0.00268542s
#Calculated fast perft for depth 3 in 0.141039s
#There are 511920 leaves
#Number of traveled states: 518482 (3676171.780730 states/sec)

	#InternationalCheckers
#Preprocessing took: 0.0168718s
#Calculated 1000 games in 4.955025s
#Time for one game: 0.004955s (201.815329 games/sec)
#Avarage number of turns in game: 93.638000
#Avarage number of moves in one state_: 7.493000
#Number of traveled player states: 93638 (18897.583789 states/sec)
#Number of traveled keeper states: 277935 (56091.543501 states/sec)
#Number of traveled states: 371573 (74989.127290 states/sec)
#Number of calculated moves: 745997 (150553.630083 moves/sec)
#Avarage player scores: 
	#white : 48.350000 (Min: 0, Max: 100)
	#black : 51.650000 (Min: 0, Max: 100)
#Preprocessing took: 0.0174954s
#Calculated fast perft for depth 6 in 2.339037s
#There are 167140 leaves
#Number of traveled states: 199271 (85193.605233 states/sec)

	#PaperSoccer
#Preprocessing took: 0.0328401s
#Calculated 100 games in 3.167890s
#Time for one game: 0.031679s (31.566755 games/sec)
#Avarage number of turns in game: 58.450000
#Avarage number of moves in one state_: 140.180000
#Number of traveled player states: 5845 (1845.076829 states/sec)
#Number of traveled keeper states: 5745 (1813.510074 states/sec)
#Number of traveled states: 11590 (3658.586903 states/sec)
#Number of calculated moves: 825052 (260442.143340 moves/sec)
#Avarage player scores: 
	#white : 45.000000 (Min: 0, Max: 100)
	#black : 55.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0192648s
#Calculated fast perft for depth 5 in 0.177206s
#There are 82026 leaves
#Number of traveled states: 88081 (497053.731655 states/sec)

	#Reversi
#Preprocessing took: 0.0239201s
#Calculated 10000 games in 9.105227s
#Time for one game: 0.000911s (1098.270284 games/sec)
#Avarage number of turns in game: 60.414200
#Avarage number of moves in one state_: 7.982800
#Number of traveled player states: 604142 (66351.120574 states/sec)
#Number of traveled keeper states: 624142 (68547.661141 states/sec)
#Number of traveled states: 1228284 (134898.781715 states/sec)
#Number of calculated moves: 5120300 (562347.333367 moves/sec)
#Avarage player scores: 
	#white : 51.900000 (Min: 0, Max: 100)
	#black : 48.100000 (Min: 0, Max: 100)
#Preprocessing took: 0.0114256s
#Calculated fast perft for depth 8 in 2.571391s
#There are 390216 leaves
#Number of traveled states: 455221 (177032.982697 states/sec)

	#TheMillGame
#Preprocessing took: 0.0134087s
#Calculated 10000 games in 11.455614s
#Time for one game: 0.001146s (872.934426 games/sec)
#Avarage number of turns in game: 51.000000
#Avarage number of moves in one state_: 20.781100
#Number of traveled player states: 510000 (44519.655714 states/sec)
#Number of traveled keeper states: 1093982 (95497.454897 states/sec)
#Number of traveled states: 1603982 (140017.110611 states/sec)
#Number of calculated moves: 10847476 (946913.523309 moves/sec)
#Avarage player scores: 
	#white : 50.000000 (Min: 50, Max: 50)
	#black : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0147618s
#Calculated fast perft for depth 5 in 5.636173s
#There are 5140800 leaves
#Number of traveled states: 5408545 (959612.958960 states/sec)

	#TicTacToe
#Preprocessing took: 0.00218446s
#Calculated 1000000 games in 6.522655s
#Time for one game: 0.000007s (153311.798686 games/sec)
#Avarage number of turns in game: 7.753578
#Avarage number of moves in one state_: 5.415552
#Number of traveled player states: 7753578 (1188714.989431 states/sec)
#Number of traveled keeper states: 8499784 (1303117.173482 states/sec)
#Number of traveled states: 16253362 (2491832.162913 states/sec)
#Number of calculated moves: 42527080 (6519903.127659 moves/sec)
#Avarage player scores: 
	#xplayer : 64.813050 (Min: 0, Max: 100)
	#oplayer : 35.186950 (Min: 0, Max: 100)
#Preprocessing took: 0.00204336s
#Calculated fast perft for depth 10 in 0.302205s
#There are 0 leaves
#Number of traveled states: 549946 (1819775.177261 states/sec)
