log() {
   level=$1
   echo "[$level]" "${@:2}"
}

log_info() {
   log "INFO" $@
}

log_fatal() {
   log "FATAL" $@
}

check_env() {
   env_key=$1
   eval env_value='$'$env_key
   if [ -z $env_value ]; then
      log_fatal "environment $env_key is not set"
      exit 1
   else
      log_info "environment $env_key is set to $env_value"
   fi
}

config_git() {
   dir=$1
   mkdir -p $dir
   cd $dir
   git init
   git config --local user.name chxia
   git config --local user.email chxia1024@gmail.com
   cd -
}

# clone git repos to dir
clone_git_repos() {
   dir=$1
   shift
   repos=("${@}")
   log_info "will clone $repos to $dir"
   config_git $dir
   cd $dir
   for repo in ${repos[@]}
   do
      git_uri=`echo $repo | awk '{print $1}'`
      local_path=`echo $repo | awk '{print $2}'`
      log_info "Git clone from $git_uri to $dir/$local_path"
      git clone $git_uri $local_path
   done 
   cd -
}

check_ret() {
   if [ $1 -ne 0 ]; then
      log_fatal "Failed - $2"
      exit 2
   else
      log_info "Completed - $2"
   fi
}
