SpeedReader
===========

This is a cross-platform SpeedReader developed with Qt, which I did as my first big C++ project, so don't be too hard on me. It is licensed under the GPL v2 terms. [Speed reading](http://en.wikipedia.org/wiki/Speed_reading) is a technique to read any kinds of texts super fast. If you have any ideas on how to improve the SpeedReader or its code, simply write an [email](mailto:niklas.baudy@vanniktech.de) to me or fork the project. If you have any problems or you have found any issues or bugs, feel free to report them.

## Download
[![Get it for Windows](http://vanniktech.de/img/windows_icon.png)](http://vanniktech.de/SpeedReader/Windows?ref=github)
[![Get it for Mac](http://vanniktech.de/img/apple_icon.png)](http://vanniktech.de/SpeedReader/Mac?ref=github)
[![Get it for Linux](http://vanniktech.de/img/linux_icon.png)](http://vanniktech.de/SpeedReader/Linux?ref=github)
[![Get it on Google Play](https://developer.android.com/images/brand/en_generic_rgb_wo_45.png)](https://play.google.com/store/apps/details?id=com.vanniktech.speedreader)

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

## Building

I use Qt Creator to build and develop SpeedReader. I am using Qt v5.2.1. If you want to build the SpeedReader with Qt 4.x.x, you may run into some problems, because the application does use some new Qt functions. Also building the app with qmake is not recommended, since you may run into some errors depending on your system environment.

Some guys have contacted me about the two mentioned issues and they want to "port" the application. I will keep you posted about that. If you like, feel free to fork and mess around!

## License
GPL v2

For more information see the [LICENSE file](LICENSE).