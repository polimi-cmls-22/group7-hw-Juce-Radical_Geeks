<div id="top"></div>

<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]

<!-- PROJECT LOGO -->

<br />
<div align="center">
  <a href="https://github.com/polimi-cmls-22/group7-HW-SC-Radical_Geeks">
    <img src="logo.png" alt="Logo" width="640" height="120">
  </a>

<h3 align="center">YetAnotherAutoWah</h3>

  <p align="center">
   AutoWahWah plugin
    <br />
    <a href="https://github.com/polimi-cmls-22/group7-HW-SC-Radical_Geeks"><strong>Explore the docs»</strong></a>
    <br />
    <br />
 
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contact">Contact</a></li>

  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About The Project
The aim of this project is to implement a wah-wah effect plugin. The wah-wah effect consists in a passband filter, whose central frequency varies over time in a prescribed range and according to a modulating function that can be modified by a knob.
The input of the plugin is a signal (e.g. an instrument, a song, a midi instrument), and the output is the signal modified after applying the wah-wah effect. The user can choose the parameters by interacting with the GUI.
<p align="right">(<a href="#top">back to top</a>)</p>

### Built With

* [Juce](https://juce.com/)
<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites
You have to download Juce.
### Installation
For Mac users:
1. Download for free Juce at [https://juce.com/get-juce]
2. Install xCode (or another IDE)
3. Save and open the project in the IDE through the Projucer button
4. Built the plugin in preferred version (i.e. stand-alone, vst3, component...)
5. Insert the newly obtnained file in your favorited daw

For Windows users: 
1. Download for free Juce at [https://juce.com/get-juce]
2. Install Visual Studio (or another IDE)
3. Save and open the project in the IDE through the Projucer button
4. Built the plugin in preferred version (i.e. stand-alone, vst3, component...)
5. Insert the newly obtnained file in your favorited daw 

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- USAGE -->
## Usage

The user can select the sweep function with the drop-down menu.
The user can control the gain and the quality of the peak with two knobs on the last row.
The user can use the plugin with the signal produced by an instrument or with an audio file in input to the plugin.


The plugin can be divided in three main parts, upper section, middle section, lower section.
The upper section is composed by (from left to rigth) :
1. the first knob sets the starting sweep frequency (values must be in the [0.5 Hz, 20kHz] range ). 
2. the second knob sets the ending sweep frequency (values must be in the [0.5 Hz, 20kHz] range). 
3. the third knob sets the velocity of the sweep function (values must be in the [0.5 Hz, 20kHz] range). 
4. the drop-down menu where you can choose the type of signal. 
  <img src="upper_section.jpg" alt="uppersection" width="600" height="200">


The middle section contains the frequency analyzer.

<img src="middle_section.jpg" alt="middlesection" width="600" height="200">

The lower section is composed by (from left to rigth) :
1. the first knob(can be chosen between -24dB and 24dB) determine whether the peak gain of the filter.
2. the second knob (can be chosen between 0.1dB and 10dB) determine the peak quality of the filter.
3. the third knob (can be chosen between 0% and 100%) determine the wet/dry ratio.
4. A botton to enable the visualization of the FFT of the input 

<img src="lower_section.jpg" alt="lowersection" width="600" height="200">

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ROADMAP -->
## Roadmap

- [✓] implement a wah-wah plugin 
- [✓] you can use the wah-wah plugin with the waveform produced by an instrument
- [✓] you can choose between a sinusoidal wave, a square wave or a sawtooth wave to control the filter
- [✓] you can choose the first and the last frequency of the filter
- [✓] you can choose the sweep velocity of the filter
- [✓] you can choose the gain and the quality of the peak 
- [✓] you can view the spectogram of the filter and eventually of the input signal.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Gerardo Cicalese - (gerardo.cicalese@mail.polimi.it) </p>
Alberto Bollino - (alberto.bollino@mail.polimi.it) </p>
Umberto Derme - (umberto.derme@mail.polimi.it) </p>
Giorgio Granello - (giorgio.granello@mail.polimi.it) </p>

Project Link: [https://github.com/polimi-cmls-22/group7-HW-SC-Radical_Geeks](https://github.com/polimi-cmls-22/group7-HW-SC-Radical_Geeks)

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/polimi-cmls-22/group7-hw-SC-Radical_Geeks.svg?style=for-the-badge
[contributors-url]: https://github.com/polimi-cmls-22/group7-hw-SC-Radical_Geeks/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/polimi-cmls-22/group7-hw-SC-Radical_Geeks.svg?style=for-the-badge
[forks-url]: https://github.com/polimi-cmls-22/group7-hw-SC-Radical_Geeks/network/members
[stars-shield]: https://img.shields.io/github/stars/polimi-cmls-22/group7-hw-SC-Radical_Geeks.svg?style=for-the-badge
[stars-url]: https://github.com/polimi-cmls-22/repo_name/stargazers
