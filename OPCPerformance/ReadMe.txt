OPCPerformance has the following command lines:- 

1) OPCPerformance.exe <OPCItemList> <Host> <OPCServer> <noRefreshs>
2) OPCPerformance.exe <OPCItemList> <Host> <OPCServer>


2) will write the namespace for <OPCServer> on <Host> to the file <OPCItemList> where it may be edited.
1) will create an OPC group for <OPCServer> on <Host>, which contains items listed in <OPCItemList>, it will perform 
<noRefreshs> refresh's and measure the time it takes these refreshs to complete.

NOTE - if the OPC server crash's this program will hang!!!