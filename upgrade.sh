echo "[MUPDATER] Start updating process..."
sshpass -p Paggio_25 ssh root@10.147.18.225 "cd /root/"
echo "[MUPDATER] Removing old package..."
sshpass -p Paggio_25 ssh root@10.147.18.225 "rm multicon*"
echo "[MUPDATER] Copyng new package..."
cd bin/ar71xx/packages/
sshpass -p Paggio_25 scp multicon_1.0-1_ar71xx.ipk root@10.147.18.225:/root
echo "[MUPDATER] Uninstall old package..."
sshpass -p Paggio_25 ssh root@10.147.18.225 "opkg remove multicon"
echo "[MUPDATER] Install new package:"
sshpass -p Paggio_25 ssh root@10.147.18.225 "opkg install multicon_1.0-1_ar71xx.ipk"
