# -group7-hw-Juce-Radical_Geeks
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

<h3 align="center">FM Synth</h3>

  <p align="center">
   AutoWahWah plugin
    <br />
    <a href="https://github.com/polimi-cmls-22/group7-HW-SC-Radical_Geeks"><strong>Explore the docs»</strong></a>
    <br />
    <br />
    <a href="https://github.com/polimi-cmls-22/group7-HW-SC-Radical_Geeks">View Demo</a>
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

<p align="right">(<a href="#top">back to top</a>)</p>

### Built With

* [Juce](https://juce.com/)
<p align="right">(<a href="#top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

### Prerequisites
You have to download Juce.
### Installation

1. Download for free Juce at [https://juce.com/get-juce]
2. Install Juce
3. Run the sclang script
4. Experiment with the plugin and use it to create new sound!
   

<p align="right">(<a href="#top">back to top</a>)</p>

 

<!-- USAGE EXAMPLES -->
## Usage

<img src="screenshot.png" alt="Logo" width="700" height="350">

The user can congifure the limits of the peak sweep with the two knobs on the first row.
The user can control the sweep velocity with the knob on the first row.
The user can select the sweep function with the drop-down menu.
The user can control the gain and the quality of the peak with two knobs on the last row.
The user can use the plugin with the signal produced by an instrument or with an audio file in input to the plugin.

The CSV file must have the following structure:
The first four rows of the CSV file will control respectively the knobs of the four modulators in the following way:
1.	the first value of the i-th row sets the ratio of the i-th modulator (values must be in the [0.1, 20] range with step=0.5). 
2.	the second value of the i-th row sets the attack of the i-th modulator (values must be in the [0.001, 2] range with step=0.01). 
3.	the third value of the i-th row sets the release of the i-th modulator (values must be in the [0.1, 10] range with step=0.01). 
4.	the last value of the i-th row sets the index of the i-th modulator (values must be in the [0.1, 20] range with step=1). 
		
The fifth row will control the knobs of the carrier in the following way:
1.	the first value sets the panning of the master output (values must be in the [-1, 1] range with step=0.1). 
2.	the second value sets the master volume (values must be in the [0.001, 2] range with step=0.01).
3.	the third value sets the master attack (values must be in the [0.001, 2] range with step=0.01). 
4.	the last value sets the master release (values must be in the [0, 1] range). 

The sixth row will control the algorithm of the FMSynth:
1.	the first value (can be chosen between 0 and 1) determine whether the left path (modulators 1 and 3) will be active ('1') or not ('0').
2.	the second value (can be chosen between 0 and 1) determine whether the right path (modulators 2 and 4) will be active ('1') or not ('0').
3.	the third value (can be chosen between 0, 1 and 2) determine whether the modulator 3 is not active ('0') or its input comes from modulator 1 ('1') or from the carrier ('2').
4.	the fourth value (can be chosen between 0, 1 and 2) determine whether the modulator 4 is not active ('0') or its input comes from modulator 2 ('1') or from the carrier ('2')	

The only value of the last row will be used to select (between sine ('0'), triangle ('1') and sawtooth ('2')) the waveshape of the carrier.


Furthermore,  the user can create the FMSynth architecture that he likes the most. This can be done thanks to 4 buttons in the GUI: 
1. 'Left path active / Left path active inactive': Activation of the left feed forward chain (enabling/disabling the modulator 3, directly connected to the carrier).
2. 'Not active / Input from 1 / Feedback': Mode of operation of the left feed forward chain (choosing whether the input of the modulator 3 is no input or the output of modulator 1 or the carrier (feedback)).
3. 'Right path active / Right path active inactive': Activation of the right feed forward chain (enabling/disabling the modulator 4, directly connected to the carrier).
4. 'Not active / Input from 2 / Feedback': Mode of operation of the right feed forward chain (choosing whether the input of the modulator 4 is no input or the output of modulator 2 or the carrier (feedback)).

## Play demo
Thanks to the 'Play demo / Stop demo' button the user can listen to a demo song created by the Team, in order to let him hear what sounds can be created via the FMSynth.
DISCLAIMER: .

## Real time scope and frequency scope
.

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
