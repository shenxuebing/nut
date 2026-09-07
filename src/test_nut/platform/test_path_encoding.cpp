/**
 * 路径编码适配层单元测试
 *
 * 验证窄字符路径按显式编码约定转换(默认 UTF-8, 定义 NUT_PATH_GGBK 则为 GBK)，
 * 与系统 ACP 设置无关，中文文件/目录名在 GBK 或 UTF-8 系统下均不乱码。
 *
 * 注：本文件的中文字符串一律使用字节转义而非中文字面量，
 *     使测试结果不依赖源码保存编码和编译器字符集选项
 */
#include <string>
#include <vector>

#include <nut/unittest/unittest.h>
#include <nut/platform/os.h>
#include <nut/platform/path.h>
#include <nut/platform/savefile.h>
#include <nut/util/string/string_utils.h>


using namespace std;
using namespace nut;

namespace
{

/* "数字证书" 的 UTF-8 字节: U+6570 U+5B57 U+8BC1 U+4E66 */
const char *CHINESE_UTF8 = "\xe6\x95\xb0\xe5\xad\x97\xe8\xaf\x81\xe4\xb9\xa6";
/* "数字证书" 的 GBK(936) 字节 */
const char *CHINESE_GBK = "\xca\xfd\xd7\xd6\xd6\xa4\xca\xe9";
/* "数字证书" 的 UTF-16 码元 */
const wchar_t *CHINESE_WSTR = L"\x6570\x5B57\x8BC1\x4E66";

#ifdef NUT_PATH_GGBK
const char *chinese_path_text()
{
    return CHINESE_GBK;
}
#else
const char *chinese_path_text()
{
    return CHINESE_UTF8;
}
#endif

} // namespace


class TestPathEncoding : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_path_to_wstr);
        NUT_REGISTER_CASE(test_wstr_roundtrip);
        NUT_REGISTER_CASE(test_chinese_dir_lifecycle);
        NUT_REGISTER_CASE(test_chinese_path_attrs);
        NUT_REGISTER_CASE(test_chinese_savefile);
        NUT_REGISTER_CASE(test_chinese_cwd);
    }

    /* 互不冲突的中文临时目录(get_tmpDir() 结果以分隔符结尾) */
    string tmp_root() const
    {
        return Path::get_tmpDir() + "nut_test_" + chinese_path_text();
    }

    /* 清理历史残留 */
    void clean_root()
    {
        if (Path::exists(tmp_root()))
            NUT_TA(OS::rmtree(tmp_root()));
    }

    /* 路径编码适配: 窄字符路径 -> 宽字符路径 */
    void test_path_to_wstr()
    {
#ifdef NUT_PATH_GGBK
        // GBK 约定构建: GBK 字节按代码页 936 显式解码
        NUT_TA(path_to_wstr(CHINESE_GBK) == CHINESE_WSTR);
        NUT_TA(path_to_wstr(string(CHINESE_GBK)) == CHINESE_WSTR);
#else
        // 默认约定: UTF-8 字节按 CP_UTF8 显式解码，与系统 ACP 无关
        NUT_TA(path_to_wstr(CHINESE_UTF8) == CHINESE_WSTR);
        NUT_TA(path_to_wstr(string(CHINESE_UTF8)) == CHINESE_WSTR);
#endif

        // 纯 ASCII 路径两种约定下结果一致
        NUT_TA(path_to_wstr("E:/cpputil") == L"E:/cpputil");
        NUT_TA(path_to_wstr("").empty());
    }

    /* 宽字符路径 -> 窄字符 -> 宽字符 往返无损 */
    void test_wstr_roundtrip()
    {
        const wstring w = CHINESE_WSTR;
        const string narrow = wstr_to_path(w);
        NUT_TA(path_to_wstr(narrow) == w);
#ifdef NUT_PATH_GGBK
        NUT_TA(narrow == CHINESE_GBK);
#else
        NUT_TA(narrow == CHINESE_UTF8);
#endif
        NUT_TA(wstr_to_path(L"").empty());
    }

    /* 中文路径: 递归创建目录 -> 遍历读回名字 -> 递归删除 */
    void test_chinese_dir_lifecycle()
    {
        clean_root();

        const string deep = Path::join(tmp_root(),
            Path::join(chinese_path_text(), chinese_path_text())); // 三层中文
        NUT_TA(OS::mkdirs(deep));
        NUT_TA(Path::exists(deep));

        // listdir 读回的目录名应与写入时的编码约定一致(不含 . 和 ..)
        const vector<string> children = OS::listdir(
            Path::join(tmp_root(), chinese_path_text()),
            false, false, true);
        NUT_TA(1 == children.size());
        NUT_TA(children.at(0) == chinese_path_text());

        // 复制文件到中文路径
        const string src = Path::join(deep, "src.bin");
        {
            SaveFile sf(src);
            NUT_TA(sf.open());
            NUT_TA(sf.write("abc", 3));
            NUT_TA(sf.commit());
        }
        const string dst = Path::join(deep, string("dst_") + chinese_path_text() + ".bin");
        NUT_TA(OS::copyfile(src, dst));
        NUT_TA(Path::exists(dst) && 3 == Path::get_size(dst));

        // 重命名为另一个中文名
        const string dst2 = Path::join(deep, string("renamed_") + chinese_path_text() + ".bin");
        NUT_TA(OS::rename(dst, dst2));
        NUT_TA(Path::exists(dst2) && !Path::exists(dst));

        // 递归删除整棵中文目录树
        NUT_TA(OS::rmtree(tmp_root()));
        NUT_TA(!Path::exists(tmp_root()));
    }

    /* 中文路径的属性判断 */
    void test_chinese_path_attrs()
    {
        clean_root();

        const string sub = Path::join(tmp_root(), chinese_path_text());
        NUT_TA(OS::mkdirs(sub));

        NUT_TA(Path::exists(sub));
        NUT_TA(Path::is_dir(sub));
        NUT_TA(!Path::is_file(sub));

        // 不存在的中文路径
        const string absent = Path::join(tmp_root(),
            string("absent_") + chinese_path_text());
        NUT_TA(!Path::exists(absent));
        NUT_TA(!Path::is_dir(absent));
        NUT_TA(!Path::is_file(absent));

        // wstring 双轨接口与窄字符接口指向同一目录
        const wstring wsub = path_to_wstr(sub);
        NUT_TA(Path::exists(wsub));
        NUT_TA(Path::is_dir(wsub));
        NUT_TA(!Path::is_file(wsub));

        NUT_TA(OS::rmtree(tmp_root()));
        NUT_TA(!Path::exists(tmp_root()));
    }

    /* SaveFile 写中文文件名 */
    void test_chinese_savefile()
    {
        clean_root();
        NUT_TA(OS::mkdirs(tmp_root()));

        const string file_path = Path::join(tmp_root(),
            string("config_") + chinese_path_text() + ".json");
        {
            SaveFile sf(file_path);
            NUT_TA(sf.open());
            NUT_TA(sf.write("{\"app\":\"nut\"}", 13));
            NUT_TA(sf.commit());
        }
        NUT_TA(Path::exists(file_path));
        NUT_TA(Path::is_file(file_path));
        NUT_TA(!Path::is_dir(file_path));
        NUT_TA(13 == Path::get_size(file_path));

        // 时间戳属性可正常获取
        NUT_TA(0 != Path::get_mtime(file_path));

        NUT_TA(OS::rmtree(tmp_root()));
        NUT_TA(!Path::exists(tmp_root()));
    }

    /* 中文工作目录的 chdir/get_cwd 往返 */
    void test_chinese_cwd()
    {
        clean_root();
        NUT_TA(OS::mkdirs(tmp_root()));

        const string old_cwd = Path::get_cwd();
        Path::chdir(tmp_root());
        const string now_cwd = Path::get_cwd();
        Path::chdir(old_cwd);

        // get_cwd() 返回的路径应包含与写入约定一致编码的目录名
        NUT_TA(now_cwd.find(chinese_path_text()) != string::npos);

        NUT_TA(OS::rmtree(tmp_root()));
    }
};

NUT_REGISTER_FIXTURE(TestPathEncoding, "platform,quiet")
