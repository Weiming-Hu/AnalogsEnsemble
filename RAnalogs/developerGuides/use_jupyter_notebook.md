# Notes for Using Jupyter Notebook

To launch Jupyter notebook:

- Activate virtual environment for jupyter notebook
- `cd` in to the folder `AnalogsEnsemble/RAnalogs/examples`
- Start Jupyter notebook server `jupyter notebook`

# Convert R markdown files to ipynb

Tools to be used:

- [ipyrmd](https://github.com/chronitis/ipyrmd)
- [IRkernel](https://github.com/IRkernel/IRkernel)

Create a virtual environment with python 3.

```
virtualenv -p python3 envname
source envname/bin/activate
```

Install required tools.

```
pip install jupyter nbformat pyyaml ipyrmd 
R -e "install.packages('IRkernel')"
R -e "IRkernel::installspec()"
```

Convert Rmd to ipynb.

```
ipyrmd --t ipynb --from Rmd -o notebooks/demo-2_search-extension.ipynb demo-2_search-extension.Rmd -y
```
