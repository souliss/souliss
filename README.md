# souliss

[![Join the chat at https://gitter.im/souliss/souliss](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/souliss/souliss?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[![](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/orgs/souliss/rooms#)


Souliss is a framework to manage interconnected Things, smart homes and automated appliances. It includes a network layer that gives virtualization over the communication media, an event based protocol and datastructure and an Android user interface.
It runs over *Arduino (and compatible), Android and openHAB*. it includes all the code and drivers to get started, you just have to tune it to your needs and load it.

##How it works

The concept behind **distributed** is to have nodes that can interact directly, without a central device used as data collector. _Souliss_ is built over a *peer-to-peer network layer* that has virtualization facilities; it basically merges networks build over different communication medias and controllers, with automatic bridging and routing. The final user doesn't need to care about networking almost at all.

Nodes communication is achieved using an **event-based and stateless protocol** to have quick responses also on low bandwidth devices; it is a binary protocol to keep low resources usage on the microcontroller side.
Network and data communication is hidden at the user level, where is just required to fill-in network addresses and initialize the back-end once. Every node runs its own logic so, in case of failure, basic functionality are *still available*, data can be shared and eventual logics and input/output data can also be shared over.

![](https://lh3.googleusercontent.com/-kRMZdBxN1H0/UX2iDCxVHPI/AAAAAAAAARw/kutVZ8TrDJ0/s800/Souliss%2520Overview.png)

##User Interaction

User interaction is based on a single (or multiple) collector node, that gets data from all the nodes and provides a full state of your controlled objects. This approach brings easy interaction over internet, having a single node to be accessed. There is no restriction in direct access of other nodes, even if this is not the standard way in case of user interaction.
The main user interface is *SoulissApp for Android*, in this way the smartphone or tablet can become a Souliss node and it is able to communicate with nodes directly, using the same event-based protocol used by other nodes. 
If you need a central server to collect data and schedule actions, use the [openHAB Binding](https://github.com/souliss/souliss/wiki/openHAB%20Binding).


![](https://lh4.googleusercontent.com/-PSEZxGoDcgg/UVDGh5vA05I/AAAAAAAAAN0/MqxCSGceIJc/w220-h367-no/Screenshot_2013-03-25-22-45-15.png) 
![](https://lh4.googleusercontent.com/-nQo-SRuYFOw/Ue3IhgWw33I/AAAAAAAAAVs/kyFEE0RjT48/w220-h367-no/Screenshot_2013-07-08-01-52-41.png) 
![](https://lh4.googleusercontent.com/-vrlbcY4uiCU/Ue3IiPGGvyI/AAAAAAAAAV4/YDXT9b_bC5c/w220-h367-no/Screenshot_2013-07-23-01-53-19.png)


Have a look to the [Wiki](https://github.com/souliss/souliss/wiki) and relative examples. Please notify us of any bug you may encounter on [Community](https://github.com/souliss/souliss/wiki/Community) or on [Issues](https://github.com/souliss/souliss/issues) section.
