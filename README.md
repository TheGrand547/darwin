# Poor Man's Darwin World

This is a modified C implementation of "Darwin's World", originally developed by Nick Parlante outlined [here](https://users.cs.duke.edu/~ola/darwin.html). Basic overview is that there is a tiled board that has a set of creatures on it. Each on of these creatures belongs to a "species" that determines  what code they are will execute. Every round, each creature takes one (1) action, which continues until a number of cycles is complete (default is 1000). 

## Installation

Download, and unzip the folder in any location. Run "setup.sh", contents below, if gcc is installed or use whatever native c compiler you have. 

```bash
$ gcc main.c -o darwin
$ gcc comp.c -o compiler
```

## Usage
To run the simulation you will need at least one compiled .cdwn(compiled darwin) file. To compile to a .cdwn file simply run the compiled "comp.c" file with arguments like so. 
```bash
$ ./compiler inputfile.darwin outputfile.cdwn
```
Where "inputfile.darwin" is a plain text document of the form discussed in the Commands section. Note that the compiler will not tell you if you forget a argument which can lead to unexpected behavior.

To run the simulation simply follow the usage pattern listed below.
```console
$ ./darwin creature1.cdwn creature2.cdwn ...
```
For additionally parameters and their usage look here.
```console
usage: ./darwin c1 [c2 ...] [-w width] [-h height] [-n num] [{-c | -cycles} cycles] [-s num]
	c1, c2, etc.: *.cdwn files for compiled creatures to simulate
		(maximum amount of species is 100)
	-w width, -h height: width and height of the playing field respectively.
		(default is 10 for both)
	-n num: number of each species
		 (defualt is floor((w * h) / 4 * (nspecies)) )
	-c cycles: max amount of times the code for each animal will be executed
		 (defualt is 1000)
	-s num: the number of times the screen will be drawn
		(default is 10)

```
## Commands
These are the commands that a "*.darwin" file should be composed of, with their listed effect. Important note! any of the Instructions that jump the line of execution of the creature executing the program do NOT count as a move, ie checking the space is empty before moving does not take up that creature's action for that round.
* HOP: Moves the creature a single space in the direction they are facing.
* LEFT: Turns the creature 90 degrees to the left.
* RIGHT: Turns the creature 90 degrees to the right
* INFECT n: Infects(turns the creature to it's own species) the creature in front of it, if any. n refers to the line of the code to start on.
* IFEMPTY n: If the next space is empty jump to line n.
* IFWALL n: If the next space is the boundary of the field jump to line n.
* IFENEMY n: If the next space is creature of a different species jump to line n.
* IFSAME n: If the next space is a creature of the same species jump to line n.
* IFRANDOM n: There is a roughly 50/50 chance to jump to line n.
* GO n: Go to line n.

Example: "creatures/Flytrap.darwin"
```
IFENEMY 4
LEFT
GO 1
INFECT 1
GO 1
```

## Contributing
There isn't really much to do but if you want create pull requests I will look over them

## Contributors
Worked on by [Jakob](https://github.com/JakobCh) and [TheGrand547](https://github.com/samf547).

## License
[BSL-1.0](https://choosealicense.com/licenses/bsl-1.0/)