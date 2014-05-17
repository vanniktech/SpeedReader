SpeedReader
===========

This is a cross-platform SpeedReader developed with Qt, which I did as my first big C++ project, so don't be too hard on me. It is licensed under the GPL v2 terms. [Speed reading](http://en.wikipedia.org/wiki/Speed_reading) is a technique to read any kinds of texts super fast. If you have any ideas on how to improve the SpeedReader or its code, simply write an [email](mailto:niklas.baudy@vanniktech.de) to me or fork the project. If you have any problems or you have found any issues or bugs, feel free to report them.

[A full description and screenshots of the SpeedReader can be found here.](http://vanniktech.de/SpeedReader/)

## Features
- read multiple words
- speedread from 300 up to 3000 words per minute
- customize text style and appearance
- number grouping (10 000 won't be displayed seperately, because of the whitespace; condition: one word at a time)
- stop- delay- and break-words
- longer words get displayed longer
- option to stall at indentions
- HTTP proxy settings configuration
- RSS interface, to consume RSS v2 feeds ([this library was used](https://github.com/vanniktech/VNTRSSReader))
- command line interface ([options are described here](http://vanniktech.de/SpeedReader/#cli))
- overview to see, what you've already read of your text and what is going to come
- calculation of estimated reading time

## Plans for v1.4.0
- support RSS v1.0/v0.91
- support ATOM v1.0
- polish and enhance complete RSS section
- improve user experience

## License
GPL v2

For more information see the [LICENSE file](LICENSE).