# How to build a container of unpacker for KEKCC
Author Daniel Ferlewicz and Seisho Abe.

## Important nottes
- You need to build the container in your local machine because KEKCC has issues somehow.

## Procedure

0. **(Not mandatory) Define your secret token**  
If you specified the token, you don't need to enter your username and password in step (3).   
You can generate the token from [here](https://git.t2k.org/-/profile/personal_access_tokens).
```
export GIT_SECRET_TOKEN=<YOUR SECRET TOKEN>
```
1. **Install apptainer (if you did't)**  
Follow [apptainer page](https://apptainer.org/docs/admin/main/installation.html)
2. **Generate definition file.**
```
./configure
```
- `unpacker_container.def` is generated.
3. **Build the conftainer.**
```
build.sh
```
- `unpacker_container.sif` is created.
- If you did't set your token in step (0), you will be requested to fill your username and password.  
4. **Copy the sif file to KEKCC**
```
scp ./unpacker_container.sif <YOUR KEKCC ACCOUNT>@login.cc.kek.jp:<WHEREVER YOU WANT>
```
5. **Execute the sif file in KEKCC**
```
ssh <YOUR KEKCC ACCOUNT>@login.cc.kek.jp
cd <WHERE YOU COPIED SIF FILE>
apptainer shell --bind /gpfs/group/t2k/nd280/SuperFGDCommissioning/:/gpfs/group/t2k/nd280/SuperFGDCommissioning/ ./unpacker_container.sif
```
- You will find `/opt/unpacker` and `/gpfs/group/t2k/nd280/SuperFGDCommissioning`
