echo $GITHUB_WORKSPACE
ls $GITHUB_WORKSPACE
echo $ktag
while true; do
    for dir in $GITHUB_WORKSPACE/android-kernel/out/*/common/drivers/qaq; do
        if [ -e "$dir/qaq.ko" ]; then
            cp $dir/qaq.ko $dir/$ktag.ko
            $GITHUB_WORKSPACE/upload.sh $dir/$ktag.ko
            exit 0
        fi
    done
    sleep 1
done