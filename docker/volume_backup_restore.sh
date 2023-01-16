#!/bin/bash
SCRIPT=$(realpath "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA

usage() {
   log_warn "Usage : $0 [backup|restore] volumnName imageName localFileName"
   exit 1
}

backup_volume() {
   volume=$1
   image=$2
   file=$3
   if [ ! -f "$file" ]; then
      log_info "Begin backup volume '$volume' using image '$image' to file '$file'"
      docker run --rm -v $volume:/tmp_volume -v $(pwd):/current_dir $image /bin/bash -c "tar zcf /current_dir/${file} -C /tmp_volume ."
      check_ret $? "Backup volume '$volume'."
   else
      log_fatal "Backup file '$file' already exist"
      usage
   fi
}

restore_volume() {
   volume=$1
   image=$2
   file=$3
   if [ -f "$file" ]; then
      log_info "Begin restore volume '$volume' using image '$image' from file '$file'"
      docker run --rm -v $volume:/tmp_volume -v $(pwd):/current_dir $image /bin/bash -c "cd /tmp_volume && tar zxvf /current_dir/${file} >/dev/null"
      check_ret $? "Restore volume '$volume'."
   else
      log_fatal "Restore file '$file' does not exist"
      usage
   fi
}

main() {
   if [ $# -ne 4 ] ; then
      log_fatal "Invalid command number $#, should be 4."
      usage
   fi

   backup_or_restore=$1
   volume=$2
   image=$3
   file=$4
   if [ "$1" == "backup" ]; then
      backup_volume  $volume $image $file
   elif [ "$1" == "restore" ]; then
      restore_volume $volume $image $file
   else
      log_fatal "Only support command [backup] or [restore]"
      usage
   fi
}

main $@
