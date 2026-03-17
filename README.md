# Study of J/ψ Production in pp Collisions at √s = 13 TeV

This project studies the production of J/ψ mesons in proton–proton collisions at √s = 13 TeV using Monte Carlo simulations with Pythia8.
The simulation results are compared with experimental measurements through the transverse momentum (pT) differential cross section.

The code was developed as part of an undergraduate research project (PIBIC).

---

# Physics Motivation

The J/ψ meson is a bound state of a charm quark and an anti-charm quark.
Studying its production in high-energy proton–proton collisions helps us understand:

* Quantum Chromodynamics (QCD)
* Heavy quark production mechanisms
* Contributions from prompt and non-prompt J/ψ production

In this simulation, the transverse momentum distribution of J/ψ particles is calculated and compared with experimental data.

---

# Tools Used

* Pythia8 — event generation
* ROOT — data analysis and histogramming
* C++ — implementation language

---

# Simulation Setup

The simulation generates proton–proton collisions at:

√s = 13 TeV

Key physics settings include:

* Inclusive soft QCD processes
* Charmonium production enabled
* Multiple parton interactions (MPI)
* Initial-state radiation (ISR)
* Final-state radiation (FSR)

J/ψ particles are selected within the rapidity region:

|y| < 0.9

---

# Analysis Performed

The code performs the following steps:

1. Generate Monte Carlo events using Pythia8
2. Identify J/ψ mesons in the event record
3. Separate:

   * prompt J/ψ
   * non-prompt J/ψ from B-hadron decays
4. Fill transverse momentum histograms
5. Normalize the distributions to obtain:

d²σ / (dpT dy)

6. Compare the result with experimental data
7. Compute the ratio:

Data / Pythia8

---

# Repository Structure

Relatorioparcial.cc
Main simulation and analysis code.

jpsi_alice_style_ratio.png
Example result showing the comparison between simulation and experimental data.

---

# Example Result

![J/ψ comparison](jpsi_alice_style_ratio.png)

The figure shows:

* Differential cross section as a function of transverse momentum
* Experimental data points
* Monte Carlo prediction from Pythia8
* Ratio between data and simulation

---

# How to Compile

Make sure Pythia8 and ROOT are installed.

Example compilation command:

g++ Relatorioparcial.cc -o jpsi_analysis $(pythia8-config --cxxflags --libs) $(root-config --cflags --libs)

---

# Running the Simulation

Run the executable:

./jpsi_analysis

The program generates events, performs the analysis and saves:

* histogram outputs in a ROOT file
* a comparison plot in PNG format

---

# Output Files

jpsi_output.root
ROOT file containing the generated histograms.

jpsi_alice_style_ratio.png
Plot comparing simulation and experimental data.

---

# Author

Gustavo Sobreira Barroso
Undergraduate student in Physical Engineering – UNICAMP
