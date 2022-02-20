Ns-3 + SEM for HO Modeling and Validation
=========================================

## Paper in Question

Cite as "Sachin Nayak and Sumit Roy. 2022. LTE Handover Modeling and Validation with ns-3 using SEM. In 2022 Workshop on ns-3 (WNS3 2022), June 22–23,2022, U.S. NIST, Gaithersburg, MD, USA. ACM, New York, NY, USA, 8 pages.https://doi.org/10.1145/3460797.3460800".

## Cloning Code

Clone the branch 'lena-sim' of this repository using the below commands.

```shell
git clone --branch lena-sim https://github.com/sachinnUW/ns-3-dev/
```

This clones the git repository for ns-3 as well as the [SEM](https://github.com/signetlabdei/sem/tree/master) python files, the existing result files and other files.


To ensure that the SEM submodule is also cloned correctly in the '/sem' directory, please run the below commands.

```shell
cd ns-3-dev
git submodule update --init --recursive
```

## Building ns-3

Command to build ns-3.

```shell
./waf configure --build-profile=optimized
./waf build
```

Command for one sample simulation run as in the paper.

```shell
./waf –run "lena-handover-many –simTime=5 –speed=50 –hysteresis=5–timeToTrigger=1024 –a3Offset=3"
```

## Installing SEM

SEM is developed using [poetry](https://python-poetry.org/docs/). To correctly install SEM, we need to install the poetry environment using the below command.

```shell
curl -sSL https://install.python-poetry.org | python3 -
```

To install SEM using poetry, run the below commands.

```shell
cd sem
poetry install
```

## Using the SEM scripts

The directory 'sem-results' has the result files and sem scripts for each set of experiments as different folders. To use these sem scripts use the below commands.

```shell
cd sem-results
jupyter notebook
```

This opens the [Jupyter](https://jupyter.org/) interface on your computer. Browse to any of the 'lte-results-\*' folders, start the 'Lte-analysis.ipynb' notebook within the folder and run the cells in them to reproduce the simulation results of the paper.

## Author

Sachin Nayak (sachinn@uw.edu)
