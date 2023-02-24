#   Define an environment variable G4INCLUDES that points to 
# your installation path, where the includes are. It is usually
# something like: /path/to/geant4-install/include/Geant4
#It should be added in the ~/.bashrc file (you can add in the last line of the file)

#Please change the "MyDetector.cpp" to "OpticalFiber.cpp" to change to Optical fiber simulation.
#Also, include here any .cpp or .cc files that are added to the simulation. THIS IS VERY IMPORTANT!
#IF NOT ADDED HERE, THE SIMULATION WILL NOT WORK PROPERLY.

#g++ -o simulationTeflon\
g++ -o Tefsimulation\
    main.cpp \
    source/TeflonDetector.cpp \
    source/MyPrimaryGenerator.cpp \
    source/MyActionInitialization.cpp \
    source/MySensorDetector.cpp \
    source/MyHit.cpp \
    source/run.cpp \
    `geant4-config --libs` -I${G4INCLUDES}
    
