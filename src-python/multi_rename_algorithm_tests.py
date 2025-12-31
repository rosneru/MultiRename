import unittest
import multi_rename_algorithm as mr


class TestMultiRename(unittest.TestCase):
    def setUp(self):
        self.files = [mr.FileNode('My 1st file is older than.md', '1978-06-18-12-00-00'),
                      mr.FileNode('my 2nd one and even more than.txt', '1983-09-06-21-22-23'),
                      mr.FileNode('My 3rd attempt.doc', '2017-11-23-22-36-43')]

    def test_apply(self):
        self.assertTrue(mr.create_new_names(self.files, 'Abc', 'def'))
        self.assertEqual(self.files[0].new_name, 'Abc.def')
        self.assertEqual(self.files[1].new_name, 'Abc.def')
        self.assertEqual(self.files[2].new_name, 'Abc.def')

    def test_name_1(self):
        mr.create_new_names(self.files, '[N]', '[E]')
        self.assertEqual(self.files[0].new_name, 'My 1st file is older than.md')
        self.assertEqual(self.files[1].new_name, 'my 2nd one and even more than.txt')
        self.assertEqual(self.files[2].new_name, 'My 3rd attempt.doc')

    def test_name_2(self):
        mr.create_new_names(self.files, 'Abc - [N]', '[E]')
        self.assertEqual(self.files[0].new_name, 'Abc - My 1st file is older than.md')
        self.assertEqual(self.files[1].new_name, 'Abc - my 2nd one and even more than.txt')
        self.assertEqual(self.files[2].new_name, 'Abc - My 3rd attempt.doc')

    def test_name_3(self):
        mr.create_new_names(self.files, '[N] - Def', '[E]')
        self.assertEqual(self.files[0].new_name, 'My 1st file is older than - Def.md')
        self.assertEqual(self.files[1].new_name, 'my 2nd one and even more than - Def.txt')
        self.assertEqual(self.files[2].new_name, 'My 3rd attempt - Def.doc')

    def test_name_4(self):
        mr.create_new_names(self.files, 'Aa[N]Bb', '[E]')
        self.assertEqual(self.files[0].new_name, 'AaMy 1st file is older thanBb.md')
        self.assertEqual(self.files[1].new_name, 'Aamy 2nd one and even more thanBb.txt')
        self.assertEqual(self.files[2].new_name, 'AaMy 3rd attemptBb.doc')

    def test_name_5_create_empty_names(self):
        mr.create_new_names(self.files, '', '[E]')
        self.assertEqual(self.files[0].new_name, '.md')
        self.assertEqual(self.files[1].new_name, '.txt')
        self.assertEqual(self.files[2].new_name, '.doc')

    def test_name_6_error_because_unclosed_name_command(self):
        self.assertFalse(mr.create_new_names(self.files, 'Abc - [N', '[E]'))
        # Should return False because of a parse error: bracket '[' not closed

    def test_name_7_error_because_double_closed_name_command(self):
        self.assertFalse(mr.create_new_names(self.files, 'Abc - [N]]', '[E]'))
        # Should return False because of a parse error: bracket ']' at the end
        # is too much.

    def test_name_part_1(self):
        self.assertTrue(mr.create_new_names(self.files, '[N4-6]', '[E]'))
        self.assertEqual(self.files[0].new_name, '1st.md')
        self.assertEqual(self.files[1].new_name, '2nd.txt')
        self.assertEqual(self.files[2].new_name, '3rd.doc')

    def test_name_part_2(self):
        mr.create_new_names(self.files, 'Abc [N4-6]', '[E]')

        self.assertEqual(self.files[0].new_name, 'Abc 1st.md')
        self.assertEqual(self.files[1].new_name, 'Abc 2nd.txt')
        self.assertEqual(self.files[2].new_name, 'Abc 3rd.doc')

    def test_name_part_3(self):
        mr.create_new_names(self.files, 'Abc [N4-6] [N4-4]', '[E]')

        self.assertEqual(self.files[0].new_name, 'Abc 1st 1.md')
        self.assertEqual(self.files[1].new_name, 'Abc 2nd 2.txt')
        self.assertEqual(self.files[2].new_name, 'Abc 3rd 3.doc')

    def test_name_part_3_error(self):
        mr.create_new_names(self.files, 'Abc [N4X6]', '[E]')

    def test_name_part_4(self):
        mr.create_new_names(self.files, '[N9-10]', '[E]')

        self.assertEqual(self.files[0].new_name, 'il.md')
        self.assertEqual(self.files[1].new_name, 'ne.txt')
        self.assertEqual(self.files[2].new_name, 'tt.doc')

    def test_ext_basic(self):
        mr.create_new_names(self.files, '[N]', '[E].renamed')
        self.assertEqual(self.files[0].new_name, 'My 1st file is older than.md.renamed')
        self.assertEqual(self.files[1].new_name, 'my 2nd one and even more than.txt.renamed')
        self.assertEqual(self.files[2].new_name, 'My 3rd attempt.doc.renamed')

    def test_counter_basic(self):
        mr.create_new_names(self.files, '[C]', '[E]')
        self.assertEqual(self.files[0].new_name, '1.md')
        self.assertEqual(self.files[1].new_name, '2.txt')
        self.assertEqual(self.files[2].new_name, '3.doc')

    def test_counter_multiple(self):
        mr.create_new_names(self.files, '[C][C]', '[E]')
        self.assertEqual(self.files[0].new_name, '11.md')
        self.assertEqual(self.files[1].new_name, '22.txt')
        self.assertEqual(self.files[2].new_name, '33.doc')

    def test_counter_also_extension(self):
        mr.create_new_names(self.files, '[C][C]', '[C]')
        self.assertEqual(self.files[0].new_name, '11.1')
        self.assertEqual(self.files[1].new_name, '22.2')
        self.assertEqual(self.files[2].new_name, '33.3')

    def test_counter_and_apply(self):
        mr.create_new_names(self.files, 'New name [C]-[C][C]', '[E]')
        self.assertEqual(self.files[0].new_name, 'New name 1-11.md')
        self.assertEqual(self.files[1].new_name, 'New name 2-22.txt')
        self.assertEqual(self.files[2].new_name, 'New name 3-33.doc')

    def test_counter_mixed_1(self):
        mr.create_new_names(self.files, 'New[C] [N]', '[E]')
        self.assertEqual(self.files[0].new_name, 'New1 My 1st file is older than.md')
        self.assertEqual(self.files[1].new_name, 'New2 my 2nd one and even more than.txt')
        self.assertEqual(self.files[2].new_name, 'New3 My 3rd attempt.doc')

    def test_counter_mixed_2(self):
        mr.create_new_names(self.files, '[N4-6] New[C] [N8-29]', '[E]')
        self.assertEqual(self.files[0].new_name, '1st New1 file is older than.md')
        self.assertEqual(self.files[1].new_name, '2nd New2 one and even more than.txt')
        self.assertEqual(self.files[2].new_name, '3rd New3 attempt.doc')

    def test_counter_settings_1(self):
        counter = mr.Counter(2, 2, 3)
        mr.create_new_names(self.files, '[C]', '[E]', counter)
        self.assertEqual(self.files[0].new_name, '002.md')
        self.assertEqual(self.files[1].new_name, '004.txt')
        self.assertEqual(self.files[2].new_name, '006.doc')

    def test_some_files_no_extensions(self):
        files = [mr.FileNode('My 1st file is older than.md', '1978-06-18-12-00-00'),
                 mr.FileNode('my 2nd one and even more than', '1983-09-06-21-22-23'),
                 mr.FileNode('My 3rd attempt', '2017-11-23-22-36-43'),
                 mr.FileNode('File.four', '2024-05-07-17-59-00')]

        mr.create_new_names(files, '[N]', '[E]')
        self.assertEqual(files[0].new_name, 'My 1st file is older than.md')
        self.assertEqual(files[1].new_name, 'my 2nd one and even more than')
        self.assertEqual(files[2].new_name, 'My 3rd attempt')
        self.assertEqual(files[3].new_name, 'File.four')

    def test_date_1(self):
        mr.create_new_names(self.files, '[YMD]-[hms]', '[E]')
        self.assertEqual(self.files[0].new_name, '19780618-120000.md')
        self.assertEqual(self.files[1].new_name, '19830906-212223.txt')
        self.assertEqual(self.files[2].new_name, '20171123-223643.doc')

    def test_date_2(self):
        mr.create_new_names(self.files, '[Y]-[M]-[D] - [hms]', '[E]')
        self.assertEqual(self.files[0].new_name, '1978-06-18 - 120000.md')
        self.assertEqual(self.files[1].new_name, '1983-09-06 - 212223.txt')
        self.assertEqual(self.files[2].new_name, '2017-11-23 - 223643.doc')

    def test_date_3(self):
        mr.create_new_names(self.files, '[YYY]', '[E]')
        self.assertEqual(self.files[0].new_name, '197819781978.md')
        self.assertEqual(self.files[1].new_name, '198319831983.txt')
        self.assertEqual(self.files[2].new_name, '201720172017.doc')

    def test_date_4(self):
        mr.create_new_names(self.files, '[Y][Y][Y]', '[E]')
        self.assertEqual(self.files[0].new_name, '197819781978.md')
        self.assertEqual(self.files[1].new_name, '198319831983.txt')
        self.assertEqual(self.files[2].new_name, '201720172017.doc')


if __name__ == '__main__':
    unittest.main()
