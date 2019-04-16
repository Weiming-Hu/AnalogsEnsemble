# Please make sure you have already installed the
# following packages:
#
# pip install jupyter nbformat pyyaml ipyrmd 
# R -e "install.packages('IRkernel')"
# R -e "IRkernel::installspec()"
#
# Weiming: I have set up the environment on Sapphire.
#

source venv/bin/activate

echo pyrmd --t ipynb --from Rmd -y demo-1_AnEn-basics.Rmd
ipyrmd --t ipynb --from Rmd -y demo-1_AnEn-basics.Rmd

echo pyrmd --t ipynb --from Rmd -y demo-2_search-extension.Rmd
ipyrmd --t ipynb --from Rmd -y demo-2_search-extension.Rmd

echo pyrmd --t ipynb --from Rmd -y demo-3_operational-search.Rmd
ipyrmd --t ipynb --from Rmd -y demo-3_operational-search.Rmd

echo ipyrmd --t ipynb --from Rmd -y demo-4_SEA2019.Rmd
ipyrmd --t ipynb --from Rmd -y demo-4_SEA2019.Rmd

deactivate
