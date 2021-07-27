# worldsensing_challenge
Challenge for position at worldsensing

In this repo you will find a possible solution for the assignment of Worldsensing.

To run the same simulations as the ones from the results shared on the document, you will need to simulate the 5 nodes of the network.

- tserver.c and tserver.h files correspond to the server or atomic clock of the network.

- tclient_1.c and tclient_1.h files correspond to the first node of the network, which is connected directly to the time source or server.

- tclient_i.c and tclient_i.h files correspond to the nodes between the first node and the end node. There could be more than one or two tclient_i running.

- tclient_end.c and tclient_end.c files correspond to the end node of the network.

Once the simulation is finished, you will find new .cvs file in the directory of the project. These files are:
- server.csv
- client_1.csv
- client_i.csv
- client_ii.csv
- client_end.csv

The python script "file data_reader.py" should be useful for generating interesting plots.

Thanks for taking the time to read.
Luca