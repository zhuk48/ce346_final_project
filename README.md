## Initial Setup

Inside the original nu-microbit-base repository, run the following commands:

`$ git remote rename origin upstream`
This is to prevent us from comitting to the main branch on accident!

`$ git remote add origin https://github.com/zhuk48/ce346_final_project.git `
This sets this fork as the main branch for pulls and commits.

## Before making changes

Get most up to date code from Github:
```
$ git pull
```

## AFter making changes

After making changes, run the following commands to update code on GitHub
```
$ git add .
$ git commit -m "COMMIT MESSAGE"
$ git push origin main
```
