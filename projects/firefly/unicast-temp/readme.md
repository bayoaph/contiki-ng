1. Copy unicast-temp.service to /etc/systemd/system
2. Enable service by "sudo systemctl enable unicast-temp" (service starts at next reboot)
3. Check service status by "sudo systemd disable unicast-temp"
4. Stop service by "sudo systemctl stop unicast-temp" or disable it by "sudo systemctl disable unicast-temp"

To start service immediately, "sudo systemd start unicast-temp"
