import xml.etree.ElementTree as ET

m = 0

with open('cvendors.txt', 'w') as f:
	for x in ET.parse('vendors.xml').getroot():

		ln = len(x.attrib['mac_prefix'])

		if ln > m:

			print(x.attrib['mac_prefix'])

			m = ln

		f.write(x.attrib['mac_prefix'].replace(':', '') + '\t' + x.attrib['vendor_name'] + '\n')

print('Well done, max mac len is %d' % m)