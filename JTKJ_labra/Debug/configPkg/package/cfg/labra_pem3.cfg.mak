# invoke SourceDir generated makefile for labra.pem3
labra.pem3: .libraries,labra.pem3
.libraries,labra.pem3: package/cfg/labra_pem3.xdl
	$(MAKE) -f /home/student/JTK_LABRA_GIT/JTKJ_labra/src/makefile.libs

clean::
	$(MAKE) -f /home/student/JTK_LABRA_GIT/JTKJ_labra/src/makefile.libs clean

