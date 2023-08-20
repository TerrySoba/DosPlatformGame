import xml.etree.ElementTree as ET
import sys


def parse_junit_xml(xml_file_path):
    # Parse the XML file
    tree = ET.parse(xml_file_path)
    root = tree.getroot()

    # Get all tests
    test_cases = root.findall(".//testcase")

    # Get failed tests
    failed_test_cases = root.findall(".//testcase/failure/..")

    for failed_test in failed_test_cases:
        # get parent element of failed_test
        failure = failed_test.find("./failure")
        print("Failed test: {}".format(failed_test.attrib["name"]))
        print("  Error message: {}".format(failure.attrib["message"]))


    print("\nNumber of tests: {}".format(len(test_cases)))
    print("Number of failed tests: {}".format(len(failed_test_cases)))

    if len(failed_test_cases) > 0:
        sys.exit(1) # exit with error code 1 to signal that the build failed

if __name__ == "__main__":
    parse_junit_xml("source/JUNIT.XML")