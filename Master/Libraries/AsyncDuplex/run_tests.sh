#!/usr/bin/env bash

# It looks like the unittest library itself leaks
export ASAN_OPTIONS=detect_leaks=0

bundle install
bundle exec ensure_arduino_installation.rb

git clone https://github.com/coddingtonbear/Regexp
mv Regexp ~/Arduino/libraries
rm -rf Regexp

bundle exec arduino_ci_remote.rb
