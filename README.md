goofyproxy
==========
goofyproxy acts as an HTTP proxy that filters received data
through an external process.  
To use it, run it and go in network preferences of your browser to configure
localhost:PORT as proxy for HTTP connections.

Build
-----
- Get the Qt Library version 4.5.0 or higher
- Run:
  qmake && make

Usage and examples
------------------
    goofyproxy PORT CONTENTTYPEPREFIX COMMAND [COMMANDARGS]
Open an HTTP proxy on port PORT.
Each response with a `Content-Type` that starts with `CONTENTTYPEPREFIX` gets
its payload sent through `COMMAND`'s stdin. Stdout is sent back to the client.

### Using sed
`goofyproxy 8080 text/html sed '{s/<p\([> ]\)/<h1\1/; s/<\/p>/<\/h1>/}'`  
would change every occurence of `<p></p>` for `<h1></h1>` in HTML resources.

### Using ImageMagick
    goofyproxy 8080 image/ convert - -colorspace Gray -sketch 0x20+120 -
![sketch](http://jturcotte.github.com/goofyproxy/goofyproxy_imagemagick_sketch.png)

    goofyproxy 8080 image/ convert - -paint 3 -
![paint](http://jturcotte.github.com/goofyproxy/goofyproxy_imagemagick_paint.png)

### Using perl
    goofyproxy 8080 text/html perl -nle ' $_=~ s/(?!<.*?)([^\w])cat(?![^<>]*?>)/\1Batman/ig; print $_'
![regexp](http://jturcotte.github.com/goofyproxy/goofyproxy_perl.png)

Tested platforms
-------------------
Linux, Mac OS X

A serious issue is often seen on Windows machines, at least on mine,
where the client connection would be closed automatically when putting
the software under medium load.

Limitations
-----------
HTTPS is not supported and pages using SPDY might cause problems.
