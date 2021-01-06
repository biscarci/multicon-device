echo "[MULTICON-UPDATER] Start updating process..."
sshpass -p Paggio_25 ssh root@10.147.18.225 "cd /root/"
echo "[MULTICON-UPDATER] Removing old package..."
sshpass -p Paggio_25 ssh root@10.147.18.225 "rm multicon*"
echo "[MULTICON-UPDATER] Copyng new package..."
cd bin/ar71xx/packages/
sshpass -p Paggio_25 scp multicon_1.0-1_ar71xx.ipk root@10.147.18.225:/root
echo "[MULTICON-UPDATER] Uninstall old package..."
sshpass -p Paggio_25 ssh root@10.147.18.225 "opkg remove multicon"
echo "[MULTICON-UPDATER] Install new package:"
sshpass -p Paggio_25 ssh root@10.147.18.225 "opkg install multicon_1.0-1_ar71xx.ipk"
