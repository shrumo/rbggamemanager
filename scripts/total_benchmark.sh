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

echo -e "\n\tEnglishCheckers-split"
../build/benchmark ../rbgParser/examples/englishCheckers-split.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/englishCheckers-split.rbg -d 8

echo -e "\n\tEnglishCheckers"
../build/benchmark ../rbgParser/examples/englishCheckers.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/englishCheckers.rbg -d 8

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
#Preprocessing took: 0.0040458s
#Calculated 2000 games in 14.809261s
#Time for one game: 0.007405s (135.050630 games/sec)
#Avarage number of turns in game: 72.600500
#Avarage number of moves in one state_: 453.456000
#Number of traveled player states: 145201 (9804.743248 states/sec)
#Number of traveled keeper states: 0 (0.000000 states/sec)
#Number of traveled states: 145201 (9804.743248 states/sec)
#Number of calculated moves: 65539229 (4425557.076366 moves/sec)
#Avarage player scores: 
	#white : 51.350000 (Min: 0, Max: 100)
	#black : 48.650000 (Min: 0, Max: 100)
#Preprocessing took: 0.00582194s
#Calculated perft for depth 2 in 3.038135s
#There are 4307152 leaves
#Number of traveled states: 4309329 (1418412.466021 states/sec)

	#Breakthrough
#Preprocessing took: 0.00475765s
#Calculated 100000 games in 28.252875s
#Time for one game: 0.000283s (3539.462788 games/sec)
#Avarage number of turns in game: 64.083180
#Avarage number of moves in one state_: 25.346370
#Number of traveled player states: 6408318 (226820.030923 states/sec)
#Number of traveled keeper states: 6508318 (230359.493711 states/sec)
#Number of traveled states: 12916636 (457179.524634 states/sec)
#Number of calculated moves: 164664091 (5828224.225544 moves/sec)
#Avarage player scores: 
	#white : 50.806000 (Min: 0, Max: 100)
	#black : 49.194000 (Min: 0, Max: 100)
#Preprocessing took: 0.00244281s
#Calculated perft for depth 5 in 6.584895s
#There are 6182818 leaves
#Number of traveled states: 6450493 (979589.322459 states/sec)

	#Chess
#Preprocessing took: 0.024709s
#Calculated 1000 games in 20.789164s
#Time for one game: 0.020789s (48.101983 games/sec)
#Avarage number of turns in game: 192.774000
#Avarage number of moves in one state_: 27.400000
#Number of traveled player states: 192774 (9272.811701 states/sec)
#Number of traveled keeper states: 385495 (18543.073997 states/sec)
#Number of traveled states: 578269 (27815.885699 states/sec)
#Number of calculated moves: 5358043 (257732.494145 moves/sec)
#Avarage player scores: 
	#white : 49.950000 (Min: 0, Max: 100)
	#black : 50.050000 (Min: 0, Max: 100)
#Preprocessing took: 0.0247636s
#Calculated perft for depth 5 in 33.720850s
#There are 4865609 leaves
#Number of traveled states: 5072213 (150417.708590 states/sec)

	#Chess-KingCapture
#Preprocessing took: 0.00906307s
#Calculated 10000 games in 20.489251s
#Time for one game: 0.002049s (488.060780 games/sec)
#Avarage number of turns in game: 110.378300
#Avarage number of moves in one state_: 33.030300
#Number of traveled player states: 1103783 (53871.319246 states/sec)
#Number of traveled keeper states: 1112642 (54303.692292 states/sec)
#Number of traveled states: 2216425 (108175.011538 states/sec)
#Number of calculated moves: 37391504 (1824932.662563 moves/sec)
#Avarage player scores: 
	#white : 49.885000 (Min: 0, Max: 100)
	#black : 50.115000 (Min: 0, Max: 100)
#Preprocessing took: 0.011281s
#Calculated perft for depth 5 in 8.247442s
#There are 4896795 leaves
#Number of traveled states: 5103860 (618841.567560 states/sec)

	#ChineseCheckers6
#Preprocessing took: 0.00930216s
#Calculated 1000 games in 7.347857s
#Time for one game: 0.007348s (136.094104 games/sec)
#Avarage number of turns in game: 201.000000
#Avarage number of moves in one state_: 53.396000
#Number of traveled player states: 201000 (27354.914869 states/sec)
#Number of traveled keeper states: 201000 (27354.914869 states/sec)
#Number of traveled states: 402000 (54709.829739 states/sec)
#Number of calculated moves: 10830822 (1474011.013814 moves/sec)
#Avarage player scores: 
	#southWestPlayer : 50.000000 (Min: 50, Max: 50)
	#southPlayer : 50.000000 (Min: 50, Max: 50)
	#southEastPlayer : 50.000000 (Min: 50, Max: 50)
	#northWestPlayer : 50.000000 (Min: 50, Max: 50)
	#northPlayer : 50.000000 (Min: 50, Max: 50)
	#northEastPlayer : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.01024s
#Calculated perft for depth 4 in 2.845588s
#There are 336969 leaves
#Number of traveled states: 351541 (123538.982568 states/sec)

	#Connect4
#Preprocessing took: 0.00301885s
#Calculated 100000 games in 10.141456s
#Time for one game: 0.000101s (9860.516949 games/sec)
#Avarage number of turns in game: 21.324400
#Avarage number of moves in one state_: 6.537100
#Number of traveled player states: 2132440 (210269.607631 states/sec)
#Number of traveled keeper states: 2231898 (220076.680578 states/sec)
#Number of traveled states: 4364338 (430346.288209 states/sec)
#Number of calculated moves: 14408733 (1420775.559626 moves/sec)
#Avarage player scores: 
	#yellow : 44.641500 (Min: 0, Max: 100)
	#red : 55.358500 (Min: 0, Max: 100)
#Preprocessing took: 0.00319128s
#Calculated perft for depth 8 in 15.576187s
#There are 5628804 leaves
#Number of traveled states: 6576565 (422219.185924 states/sec)

	#DoubleChess
#Preprocessing took: 0.0231379s
#Calculated 100 games in 33.458922s
#Time for one game: 0.334589s (2.988739 games/sec)
#Avarage number of turns in game: 195.320000
#Avarage number of moves in one state_: 98.170000
#Number of traveled player states: 19532 (583.760586 states/sec)
#Number of traveled keeper states: 39048 (1167.042975 states/sec)
#Number of traveled states: 58580 (1750.803561 states/sec)
#Number of calculated moves: 1930415 (57695.074362 moves/sec)
#Avarage player scores: 
	#white : 54.000000 (Min: 0, Max: 100)
	#black : 46.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.022902s
#Calculated perft for depth 3 in 11.713576s
#There are 389450 leaves
#Number of traveled states: 394707 (33696.542475 states/sec)

	#EnglishCheckers-split
#Preprocessing took: 0.0184898s
#Calculated 10000 games in 6.245293s
#Time for one game: 0.000625s (1601.205968 games/sec)
#Avarage number of turns in game: 69.990900
#Avarage number of moves in one state_: 4.453100
#Number of traveled player states: 699909 (112069.846795 states/sec)
#Number of traveled keeper states: 1360096 (217779.383243 states/sec)
#Number of traveled states: 2060005 (329849.230038 states/sec)
#Number of calculated moves: 3517207 (563177.283955 moves/sec)
#Avarage player scores: 
	#white : 50.780000 (Min: 0, Max: 100)
	#black : 49.220000 (Min: 0, Max: 100)
#Preprocessing took: 0.0187076s
#Calculated perft for depth 8 in 4.061297s
#There are 838248 leaves
#Number of traveled states: 1063460 (261852.285306 states/sec)

	#EnglishCheckers
#Preprocessing took: 0.0163244s
#Calculated 10000 games in 6.365752s
#Time for one game: 0.000637s (1570.906324 games/sec)
#Avarage number of turns in game: 67.729600
#Avarage number of moves in one state_: 4.601200
#Number of traveled player states: 677296 (106396.856979 states/sec)
#Number of traveled keeper states: 1337218 (210064.421310 states/sec)
#Number of traveled states: 2014514 (316461.278289 states/sec)
#Number of calculated moves: 3487099 (547790.587238 moves/sec)
#Avarage player scores: 
	#white : 49.445000 (Min: 0, Max: 100)
	#black : 50.555000 (Min: 0, Max: 100)
#Preprocessing took: 0.016242s
#Calculated perft for depth 8 in 4.200414s
#There are 845931 leaves
#Number of traveled states: 1071628 (255124.386222 states/sec)

	#Go
#Preprocessing took: 0.010242s
#Calculated 200 games in 12.778579s
#Time for one game: 0.063893s (15.651193 games/sec)
#Avarage number of turns in game: 474.335000
#Avarage number of moves in one state_: 148.615000
#Number of traveled player states: 94867 (7423.908448 states/sec)
#Number of traveled keeper states: 783990 (61351.892484 states/sec)
#Number of traveled states: 878857 (68775.800932 states/sec)
#Number of calculated moves: 13922430 (1089513.167870 moves/sec)
#Avarage player scores: 
	#white : 177.595000 (Min: 0, Max: 299)
	#black : 173.115000 (Min: 0, Max: 284)
#Preprocessing took: 0.0118861s
#Calculated perft for depth 2 in 9.122279s
#There are 130682 leaves
#Number of traveled states: 131045 (14365.379996 states/sec)

	#Gomoku
#Preprocessing took: 0.00742349s
#Calculated 10000 games in 9.334112s
#Time for one game: 0.000933s (1071.339234 games/sec)
#Avarage number of turns in game: 118.515100
#Avarage number of moves in one state_: 165.998800
#Number of traveled player states: 1185151 (126969.876472 states/sec)
#Number of traveled keeper states: 1195151 (128041.215706 states/sec)
#Number of traveled states: 2380302 (255011.092178 states/sec)
#Number of calculated moves: 193039793 (20681110.399727 moves/sec)
#Avarage player scores: 
	#white : 51.270000 (Min: 0, Max: 100)
	#black : 48.730000 (Min: 0, Max: 100)
#Preprocessing took: 0.00797553s
#Calculated perft for depth 3 in 13.960748s
#There are 11239200 leaves
#Number of traveled states: 11289826 (808683.459648 states/sec)

	#Hex
#Preprocessing took: 0.00408508s
#Calculated 10000 games in 4.906036s
#Time for one game: 0.000491s (2038.305655 games/sec)
#Avarage number of turns in game: 107.490900
#Avarage number of moves in one state_: 67.514600
#Number of traveled player states: 1074909 (219099.309307 states/sec)
#Number of traveled keeper states: 1084909 (221137.614961 states/sec)
#Number of traveled states: 2159818 (440236.924268 states/sec)
#Number of calculated moves: 72256109 (14728003.556651 moves/sec)
#Avarage player scores: 
	#red : 52.010000 (Min: 0, Max: 100)
	#blue : 47.990000 (Min: 0, Max: 100)
#Preprocessing took: 0.00391449s
#Calculated perft for depth 3 in 1.178245s
#There are 1727880 leaves
#Number of traveled states: 1742522 (1478913.341399 states/sec)

	#InternationalCheckers
#Preprocessing took: 0.0184919s
#Calculated 1000 games in 7.549880s
#Time for one game: 0.007550s (132.452433 games/sec)
#Avarage number of turns in game: 93.746000
#Avarage number of moves in one state_: 7.447000
#Number of traveled player states: 93746 (12416.885826 states/sec)
#Number of traveled keeper states: 278256 (36855.684323 states/sec)
#Number of traveled states: 372002 (49272.570149 states/sec)
#Number of calculated moves: 744110 (98559.180257 moves/sec)
#Avarage player scores: 
	#white : 47.700000 (Min: 0, Max: 100)
	#black : 52.300000 (Min: 0, Max: 100)
#Preprocessing took: 0.0193311s
#Calculated perft for depth 6 in 3.445785s
#There are 166992 leaves
#Number of traveled states: 199123 (57787.418808 states/sec)

	#PaperSoccer
#Preprocessing took: 0.0192754s
#Calculated 100 games in 3.824371s
#Time for one game: 0.038244s (26.148092 games/sec)
#Avarage number of turns in game: 58.450000
#Avarage number of moves in one state_: 140.180000
#Number of traveled player states: 5845 (1528.356003 states/sec)
#Number of traveled keeper states: 5745 (1502.207910 states/sec)
#Number of traveled states: 11590 (3030.563913 states/sec)
#Number of calculated moves: 825052 (215735.359581 moves/sec)
#Avarage player scores: 
	#white : 45.000000 (Min: 0, Max: 100)
	#black : 55.000000 (Min: 0, Max: 100)
#Preprocessing took: 0.0193364s
#Calculated perft for depth 5 in 0.263229s
#There are 82026 leaves
#Number of traveled states: 88081 (334617.162651 states/sec)

	#Reversi
#Preprocessing took: 0.0105916s
#Calculated 10000 games in 11.368594s
#Time for one game: 0.001137s (879.616248 games/sec)
#Avarage number of turns in game: 61.420600
#Avarage number of moves in one state_: 7.877000
#Number of traveled player states: 614206 (54026.557692 states/sec)
#Number of traveled keeper states: 628715 (55302.792906 states/sec)
#Number of traveled states: 1242921 (109329.350598 states/sec)
#Number of calculated moves: 5139317 (452062.673435 moves/sec)
#Avarage player scores: 
	#white : 48.350000 (Min: 0, Max: 100)
	#black : 51.650000 (Min: 0, Max: 100)
#Preprocessing took: 0.00833212s
#Calculated perft for depth 8 in 3.683323s
#There are 431044 leaves
#Number of traveled states: 500877 (135985.071467 states/sec)

	#TheMillGame
#Preprocessing took: 0.0129072s
#Calculated 10000 games in 13.588377s
#Time for one game: 0.001359s (735.923069 games/sec)
#Avarage number of turns in game: 51.000000
#Avarage number of moves in one state_: 20.781100
#Number of traveled player states: 510000 (37532.076513 states/sec)
#Number of traveled keeper states: 1093982 (80508.659075 states/sec)
#Number of traveled states: 1603982 (118040.735588 states/sec)
#Number of calculated moves: 10847476 (798290.782759 moves/sec)
#Avarage player scores: 
	#white : 50.000000 (Min: 50, Max: 50)
	#black : 50.000000 (Min: 50, Max: 50)
#Preprocessing took: 0.0122699s
#Calculated perft for depth 5 in 11.800228s
#There are 5140800 leaves
#Number of traveled states: 5408545 (458342.406689 states/sec)

	#TicTacToe
#Preprocessing took: 0.0019554s
#Calculated 1000000 games in 7.769102s
#Time for one game: 0.000008s (128715.010243 games/sec)
#Avarage number of turns in game: 7.753407
#Avarage number of moves in one state_: 5.415968
#Number of traveled player states: 7753407 (997979.861420 states/sec)
#Number of traveled keeper states: 8499067 (1093957.495957 states/sec)
#Number of traveled states: 16252474 (2091937.357377 states/sec)
#Number of calculated moves: 42525855 (5473715.861900 moves/sec)
#Avarage player scores: 
	#xplayer : 64.892800 (Min: 0, Max: 100)
	#oplayer : 35.107200 (Min: 0, Max: 100)
#Preprocessing took: 0.00203663s
#Calculated perft for depth 10 in 0.646391s
#There are 0 leaves
#Number of traveled states: 340858 (527325.141741 states/sec)
