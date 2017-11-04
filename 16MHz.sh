#!/bin/bash

# 16 MHz (Prescale 2 * 8 MHz ?? or OSCCAL?)
avrdude -B 1000 -c usbasp -p t85 -U lfuse:w:0xE1:m -U hfuse:w:0xDF:m

