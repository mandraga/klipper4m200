Find a compatible u-boot here: git@github.com:mandraga/u-boot.git
The device tree is in this repository.
It is also in the submodules.

The modified u-boot env replaces the stock env, it adds a one second wait to enter the u-boot console instead of zero, and chain loads u-boot mainline as default boot command.
