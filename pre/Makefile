#
# Root  make file.
#
#  DEPS is all the subfolders that do not end in a hyphen and have a Makefile
#
# make all    - invoke make all    on all sub-projects.
# make clean  - invoke make clean  on all sub-projects.
#
# each sub-project supports:
#
#   make all
#   make clean
#   make depclean
#
#   depclean cleans the current project and (recursively) all the projects
#   the current project depends on.
#
# there are typically target-specific targets as well.
#

DEPS=$(shell find . -mindepth 1 -maxdepth 1 -type d -not -name '*-' -exec test -f {}/Makefile \; -print)
all :
	true $(foreach DEP,$(DEPS),&& $(MAKE) -C $(DEP) all)

clean :
	find . -maxdepth 1 -regex '.*\(~\|#\|\.o\)' -print0 | xargs -0 /bin/rm -rf
	true $(foreach DEP,$(DEPS),&& $(MAKE) -C $(DEP) clean)

update :
	svn update

commit :
	svn commit -m ''



