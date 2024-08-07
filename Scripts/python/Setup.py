import os
import subprocess
import platform

# python setup
from Setup.Python import PythonConfiguration as PythonRequirements
PythonRequirements.validate()

# premake setup
from Setup.Premake import PremakeConfiguration as PremakeRequirements
os.chdir('../../') # change from scripts/python directory to root
premakeInstalled = PremakeRequirements.validate()

# other requirements
from Setup.Vulkan import VulkanConfiguration as VulkanRequirements
VulkanRequirements.validate()

# submodules and build files
print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if (premakeInstalled):
    if platform.system() == "Windows":
        print("\nRunning premake...")
        subprocess.call([os.path.abspath("./scripts/windows/Win-GenProjects.bat"), "nopause"])

    if platform.system() == "Linux":
        print("\nRunning premake...")
        subprocess.call(["sh", os.path.abspath("./scripts/linux/Linux-GenProjects.sh")])

    if platform.system() == "Darwin":
        print("\nRunning premake...")
        subprocess.call(["sh", os.path.abspath("./scripts/macosx/MacOSX-GenProjects.sh")])

    print("\nSetup completed!")
else:
    print("Project requires Premake to generate project files.")