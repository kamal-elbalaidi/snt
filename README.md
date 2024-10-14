<p align="center">
  <img src="https://raw.githubusercontent.com/kamal-elbalaidi/snt/main/simple_new_terminal.png" alt="SNT">
</p>

snt is a terminal emulator based on GTK+ and VTE.
## Customization
snt can be customized primarily by modifying config.h and
recompiling the source code. This keeps it fast, secure, and
simple. However, padding settings rely on GTK3.
## Installation
```bash
git clone https://github.com/kamal-elbalaidi/snt.git
```
then
```bash
cd snt/
sudo make install clean
```
## Options

#### `-c class`
Defines the window class.

#### `-n name`
Defines the window instance name.

#### `-e command`
Executes command instead of the shell. If this is used, it must be the last option on the command line, as in [`st`](https://st.suckless.org/) and [`xterm`](https://invisible-island.net/xterm/).

#### `-w width`
Sets the initial width of the window in pixels.

#### `-h height`
Sets the initial height of the window in pixels.

## Uninstallation
```bash
cd snt/
sudo make uninstall
```
## Author
#####  kamal El balaidi

## License

[MIT](LICENSE)
