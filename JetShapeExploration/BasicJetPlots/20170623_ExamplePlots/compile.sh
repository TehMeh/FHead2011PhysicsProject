g++ RunExample.cpp -o RunExample \
  `root-config --libs` `root-config --cflags` \
  -I$RootMacrosBase $RootMacrosBase/Code/library/* \
  -I../../CommonCode/include ../../CommonCode/library/*

g++ MakePrettyPlots.cpp -o MakePrettyPlot \
   `root-config --libs` `root-config --cflags` \
   -I$RootMacrosBase $RootMacrosBase/Code/library/* \
   -I../../CommonCode/include ../../CommonCode/library/*

g++ ./TMVAFactory/RunJetFactory.cpp -o ./TMVAFactory/RunJetFactory \
   `root-config --libs` `root-config --cflags` -lTMVA \
   -I$RootMacrosBase $RootMacrosBase/Code/library/* \
   -I../../CommonCode/include ../../CommonCode/library/*

g++ ./TMVAReader/RunReader.cpp -o ./TMVAReader/RunReader \
   `root-config --libs` `root-config --cflags` -lTMVA \
   -I$RootMacrosBase $RootMacrosBase/Code/library/* \
   -I../../CommonCode/include ../../CommonCode/library/*

g++ ./TMVAReader/RunReader.cpp -o ./TMVAReader/RunReader \
   `root-config --libs` `root-config --cflags` -lTMVA \
   -I$RootMacrosBase $RootMacrosBase/Code/library/* \
   -I../../CommonCode/include ../../CommonCode/library/*
