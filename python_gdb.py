# IMPORTANT(Ryan): QTCreator does not load user commands
class PrintList(gdb.Command):
  def __init__(self):
    super(PrintList, self).__init__("print_list", gdb.COMMAND_USER, gdb.COMPLETE_COMMAND)

  def invoke(self, argstr, from_tty):
    args = argstr.split(" ")
    if len(args) != 2:
      gdb.write("Usage: print_list <first> <val>\n")
      return

    first = gdb.parse_and_eval(args[0])
    if first.type.code != gdb.TYPE_CODE_PTR:
      gdb.write("Error: <first> is not a pointer\n")
      return

    val = args[1]

    node = first
    result_str = ""
    node_i = 0
    while node != 0:
      result_str += f"{node_i} {node[val]}\n"
      node = node["next"]
      node_i += 1
    gdb.write(result_str)

# help user-defined
PrintList()

# TODO: print tree
