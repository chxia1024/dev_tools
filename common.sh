config_git() {
   dir=$1
   mkdir -p $dir
   cd $dir
   git init
   git config --local user.name chxia1024
   git config --local user.email chxia1024@gmail.com
   cd -
}

# clone git repos to dir
clone_git_repos() {
   dir=$1
   repos=$2
   echo "will clone $repos to $dir"
   config_git $dir
   cd $dir
   for repo in ${repos[@]} 
   do
      git clone $repo
   done 
   cd -
}

