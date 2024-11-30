# mklotzpy

## This repository

This repository contains software utilities for common machine shop calculations, written by Marv Klotz, and some attempts to convert them into a format which is easier to run.

They were taken Marv's website at <http://myvirtualnetwork.com/mklotz/>, where he makes them available for free. Marv has also given personal permission to make them available on GitHub:

> "I'll tell you the same thing I've told all the people who have made a similar request - you have my full permission to recode the material into whatever language you like and distribute the result however you wish."
>
> - Marv Klotz

## Why does this repository exist?

This repository was made for three reasons.

1. **In case Marv's website becomes unavailable.** These are great utilities, and it would be a shame if they were lost for whatever reason.
1. **To make them easier to download and view.** They're available on Marv's website as a series of zip archives, which can't be viewed online without downloading them. This GitHub repository allows you to browse the contents online, and download them all at once.
1. **To make them easier to run.** These utilities were written in C for DOS, and the available binaries are compiled for DOS. It is possible to run them with an emulator, like [DOSBox](https://www.dosbox.com/), but Python scripts are much easier to run on modern devices. It will also make them easier to incorporate into other software in the future.

## How do I run the utilities?

### DOS EXE

To run the original DOS versions (.EXE files), get a DOS emulator like [DOSBox](https://www.dosbox.com/).

For Debian-based Linux distributions, DOSBox is available through APT.

`sudo apt-get install dosbox`

Then, simply run the script. During testing, this was a matter of going to the directory containing one of the executables in a terminal, and running it as so (in this case, the 3WIRE utility):

`dosbox 3WIRE.EXE`

This will bring up a DOS shell running the script. Many of the utilities, like 3WIRE mentioned above, have prompts with default values, listed in square brackets, which are used if you press enter without giving a value.

Many of the utilities also have a help file (with the extension .TXT), with some instructions from Marv on how to use them.

### Python

For each utility for which I've written a Python conversion, simply run the .py file using the Python interpreter. For example:

`python3 3wire.py`

The aim is to reproduce the functionality of Marv's originals as closely as possible, including default values.

There are long-term plans to make 'mklotzpy' usable as a Python library, allowing anyone to import it and use any of the utilities from external programs. However, that goal is a long way off, and may never happen.

## What about licensing?

The maintainers of this project are not the copyright holder of the C source files available in this repository. As far as we know, all of the C versions of the utilities here were written by Marv (his website does have 'submitted software' that were written by other people, but they are not reproduced here). 

From a legal standpoint, the only thing like licensing on Marv's website is his declaration that they are 'freeware'. This doesn't have a firm legal definition, but it's clear that he wants this software to be distributed as freely as possible.

Any files here that were written for this project are released under the permissive MIT license (as per the LICENSE file). Beyond that, you may use them for whatever purpose you'd like, with no need to seek our permission or give anything in return.

## Troubleshooting

If you're having problems with anything provided here, you're welcome to open an issue here on GitHub.

However, if you're having technical issues with a particular utility, or want help with an engineering or machining problem, we're unlikely to be of much help. You can of course open an issue here, and we'll help you as much as I can, but you'll probably have better luck asking Marv yourself. His email address is listed on his website, and he has been very responsive and helpful:

http://myvirtualnetwork.com/mklotz

## Contributing

Converting all of the utilities here is a big task, and we'd welcome any help which is offered.

If you've translated one of the utilities to Python, we're happy to consider it. Just raise a pull request with your work and I'll give it a once-over to make sure it behaves the same as Marv's version, then I'll add it in.

If you've ported one or more utilities into a language other than Python, I'm happy to take a look at it to see if it belongs here, or you can fork this project to have your own copy of the originals, and create your own project which ports the utilities to the language or platform of your choice.

I'm also happy to take requests about which utilities to prioritise. If there's a particular utility you'd like to get your hands on sooner, let me know and I'll push it to the top of the to-do list.

## Contributors

* [Andy Goulden](https://github.com/andygoulden) - creator and maintainer
* [Bradley Slavik](https://github.com/BradleySlavik) - `mandrel.py`
