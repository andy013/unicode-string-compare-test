// Unicode test2.cpp

#include <iostream>
#include <icu.h>

const int BUFFER_SIZE = 256;

UChar* string1 = (UChar*)u"\u27d9est string";
UChar* string2 = (UChar*)u"Test string";

void transliterate(UTransliterator* trans, UChar* string, UChar* buf) {

    u_strcpy(buf, string);
    int32_t limit = u_strlen(buf);
    UErrorCode status = U_ZERO_ERROR;

    utrans_transUChars(trans, buf, NULL, BUFFER_SIZE, 0, &limit, &status);

    // Remove any empty space from start and end of string
    if (buf[0] == '\u0020') {
        // std::cout << "empty space at start of string!" << "\n";
        u_strcpy(buf, &buf[1]);
    }

    int32_t trim_buf_length = u_strlen(buf);

    if (buf[trim_buf_length - 1] == '\u0020') {
        // std::cout << "empty space at end of string!" << "\n";
        buf[trim_buf_length - 1] = '\0';
    }
    return;
}

int main()
{
    // UChar* id = (UChar*)u"NFKD; [[:C:][:M:]] Remove;"; // Using rules instead of ID for filter
    UChar* id = (UChar*)u"My transliterator";

    // Rules: 
    // Remove some empty space characters that are not displayed in TF2 and replace some with empty space '\u0020' that are not normalized. 
    // Replace \u00A1 with i since they look almost the same in TF2 but won't show up as confusable (may need to do this with other characters as well?)
    // Normalize to NFKD form
    // Filter out all characters in the categories C (Other) and M (Mark)
    // Lastly remove any spaces that are followed by another space. 
    UChar* rules = (UChar*)u"\u200A > ; \u2009 > ; \u202F > ; \u205F > ; \u2006 > ; \u1680 > '\u0020' ; '\u2029' > '\u0020'; '\u2028' > '\u0020'; \u00A1 > i; ::NFKD; [[:C:][:M:]] > ; '\u0020' } '\u0020' > ; ";

    UErrorCode status = U_ZERO_ERROR;
    UParseError pe;
    UTransliterator* my_trans = utrans_openU(id, -1, UTRANS_FORWARD, rules, -1, &pe, &status);

    UChar buf1[BUFFER_SIZE];
    transliterate(my_trans, string1, buf1);

    UChar buf2[BUFFER_SIZE];
    transliterate(my_trans, string2, buf2);

    utrans_close(my_trans);

    USpoofChecker* sc = uspoof_open(&status);
    int confuse = uspoof_areConfusable(sc, buf1, -1, buf2, -1, &status);
    uspoof_close(sc);

    wprintf(L"string1 = %s\n", string1);
    wprintf(L"string2 = %s\n", string2);
    wprintf(L"trans1 = %s\n", buf1);
    wprintf(L"trans2 = %s\n", buf2);

    if (confuse != 0) {
        std::cout << "\nStrings are confusable! \n";
    }
    else {
        std::cout << "\nStrings are different. \n";
    }

    return 0;
}
