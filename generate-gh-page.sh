#!/bin/bash

# Referenced from
# https://gist.github.com/domenic/ec8b0fc8ab45f39403dd<Paste>
#

# Exit with nonzero exit code if anything fails
set -e

# SOURCE_BRANCH="master"
TARGET_BRANCH="gh-pages"

function doCompileCXX {
    echo "Compile for C++"
    mkdir buildC_gh-page
    cd buildC_gh-page
    cmake ..

    make document
    cd html

    echo -e '\n' | cat - index.html > temp && mv temp index.html
    echo -e '---' | cat - index.html > temp && mv temp index.html
    echo -e 'title: C++ Documentation' | cat - index.html > temp && mv temp index.html
    echo -e 'sidebar_link: true' | cat - index.html > temp && mv temp index.html
    echo -e '---' | cat - index.html > temp && mv temp index.html

    cd ../..

    # Update gh-pages
    rm -rf out/CXX || true
    mv buildC_gh-page/html/ out/CXX

    rm -rf buildC_gh-page
}

function doCompileR {
    echo "Compile for R"
    cd RAnalogs/releases
    rm -rf RAnEn || true

    unamestr=$(uname)
    if [[ "$unamestr" == 'Darwin' ]]; then
        tar xopf $(ls | tail -1)
    else
        tar -xvzf $(ls | tail --lines=1)
    fi

    cd RAnEn
    Rscript ../../notes/generate_site.R
    cd docs

    echo -e '\n' | cat - index.html > temp && mv temp index.html
    echo -e '---' | cat - index.html > temp && mv temp index.html
    echo -e 'title: R Documentation' | cat - index.html > temp && mv temp index.html
    echo -e 'sidebar_link: true' | cat - index.html > temp && mv temp index.html
    echo -e '---' | cat - index.html > temp && mv temp index.html

    cd ../../../../

    # Update gh-pages
    rm -rf out/R || true
    mv RAnalogs/releases/RAnEn/docs out/R
}

function doGenerateNewsPost {
    echo "Generate changelog site"

    cd out
    cp ../NEWS.md Changelog.md
    postName=Changelog.md
    
    echo -e '\n' | cat - $postName > temp && mv temp $postName
    echo -e '---' | cat - $postName > temp && mv temp $postName
    echo -e 'sidebar_link: true' | cat - $postName > temp && mv temp $postName
    echo -e 'title: Changelog' | cat - $postName > temp && mv temp $postName
    echo -e 'layout: page' | cat - $postName > temp && mv temp $postName
    echo -e '---' | cat - $postName > temp && mv temp $postName

    cd ..
}

# Save some useful information
REPO=$(git config remote.origin.url)
# SSH_REPO=${REPO/https:\/\/github.com\//git@github.com:}
# SHA=$(git rev-parse --verify HEAD)

# Clone the existing gh-pages for this repo into out/
# Create a new empty branch if gh-pages doesn't exist yet (should only happen on first deply)
git clone $REPO out
cd out
git checkout $TARGET_BRANCH || git checkout --orphan $TARGET_BRANCH
rm README.md
cp ../README.md .

cp ../README.md ./doc.md
echo -e '\n' | cat - doc.md > temp && mv temp doc.md
echo -e '---' | cat - doc.md > temp && mv temp doc.md
echo -e 'layout: default' | cat - doc.md > temp && mv temp doc.md
echo -e '---' | cat - doc.md > temp && mv temp doc.md

cd ..

# Call the compile functions
doGenerateNewsPost
doCompileCXX
doCompileR

echo "Build is done. Please deploy the document manually. The updated gh-page is in the folder out/"
exit 0
