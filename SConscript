Import(Split("env PREFIX DOCDIR CLIENTS EXAMPLESDIR SERVER"))

env.Install(dir=DOCDIR,source=Split("""BETA.README COPYRIGHT ChangeLog
FILES INFO INSTALL MACHINES TODO
"""))

if SERVER:
  env.Install(dir=EXAMPLESDIR,source="fspd.conf")
if CLIENTS:
  env.Install(dir=EXAMPLESDIR,source=Split("setup.sh setup.csh"))
env.Alias("install", EXAMPLESDIR)

# *************** Targets ****************

#Add install target
env.Alias("install", PREFIX+'/bin')

#Add build target
env.Alias("build", Split('server/ clients/ tests/ doc/ man/') )

#Change default target to build
env.Default(None)
env.Default("build")

Export("EXAMPLESDIR")
