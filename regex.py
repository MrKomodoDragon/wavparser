import re
import sys
regex = re.compile(r"<td>\'(?P<info_tag>.+)'<\/td>\n<td>(?P<desc>.+)<\/td>")
contents = open(sys.argv[1]).read()
thing = regex.findall(contents)
for i in thing:
    print(f'insert("{i[0]}", "{i[1]}", &hm);')
