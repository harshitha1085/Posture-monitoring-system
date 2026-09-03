# Posture Monitoring System

A wearable device that detects incorrect neck posture in real time and alerts the user through a buzzer, helping build better posture habits during daily activities.

## Overview

Built using a Raspberry Pi Pico and MPU6050 accelerometer/gyroscope, this system continuously tracks neck angle and flags poor posture as it happens. The goal was to create something practical enough for everyday wear, not just a lab demo.

## Features

- Real-time posture detection using accelerometer and gyroscope data
- Buzzer alert triggered when posture exceeds a calibrated threshold
- Calibration routine to set a personalized "correct posture" baseline

## My Role

I was mainly responsible for hardware integration and device setup, working closely with my teammates throughout development.

## Challenges & Approach

The biggest challenge was calibration — posture readings that looked stable during testing became unreliable once the device was worn and moved during real activities. We addressed this by testing the device on ourselves repeatedly across different movements, adjusting threshold values together as a team through several rounds of trial and error, until the readings became stable and dependable.

## Hardware Used

- Raspberry Pi Pico
- MPU6050 (accelerometer + gyroscope)
- Buzzer module

## What I Learned

This project showed how different real-world conditions are from a controlled test setup, and how much patience and teamwork it takes to get a wearable device working reliably for everyday use.
