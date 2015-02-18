grit
====

Grit Robotics Repository

This DOES NOT contain mechanical drawing or videos --- they are too big.

Edit change

# Linux Software Setup Instructions
## Cloning the GitHub repository
Open a `Terminal` and run this set of commands. It will ask you for your login details for GitHub and may also ask for a Username and Email, they don't have to match your GitHub Username and Email but should be something we can recognize.
````
sudo apt-get install git -y
git clone https://github.com/wmacevoy/grit.git ~/grit
cd ~/grit
````
## Downloading Tools and Library's
In a `Terminal` run these commands to download the needed packages and library's used to compile the code. There is over a gigabyte of packages so it may take some time and would be best on a fast stable Internet connection.
````
cd ~/grit
chmod +x ./setup/setup_dev_environment.sh && chmod +x ./setup/setup_environment_packages.sh
sudo ./setup/setup_dev_environment.sh
sudo ./setup/setup_environment_packages.sh
````
See the README inside of /setup/ for more info on each of the setup scripts.


Notes!