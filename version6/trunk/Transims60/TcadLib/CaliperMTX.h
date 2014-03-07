// Data structures and function prototypes exported by the CaliperMTX dll

#if !defined(CALIPERMTX_H)
#define CALIPERMTX_H


// null or missing values stored in a matrix cell
#define short_miss -32767
#define long_miss  -2147483647
#define flt_miss   -3.402823466e+38F
#define dbl_miss   -1.7976931348623158e+308

typedef int BOOL;
typedef struct _MATRIX *TC_MATRIX;

typedef enum _MATRIX_DIM
{
    MATRIX_ROW, 
    MATRIX_COL

} MATRIX_DIM;


// Type of matrix cache.
typedef enum _MAT_CACHE_TYPE
{
    SERIAL_CACHE, RANDOM_CACHE
} MAT_CACHE_TYPE;

// Indicates whether the matrix cache is active or not, and whether it is used
// for all the cores in the matrix or only one
typedef enum _MAT_CACHE_APPLY
{
    CACHE_NONE, CACHE_ONE, CACHE_ALL
} MAT_CACHE_APPLY;


enum _CONTROL_TYPE
{
    CONTROL_FALSE,      
    CONTROL_TRUE,       
    CONTROL_NEVER,      
    CONTROL_ALWAYS,     
    CONTROL_AUTOMATIC   
} ; typedef char CONTROL_TYPE;

#define IsControl(c)    (c == CONTROL_ALWAYS || c == CONTROL_TRUE)

// Maximum size of a matrix label
#define _MAX_FLABEL 64

// Maximum size of a matrix file name
#define _MAX_PATH   260


// Data types
enum _data_type
{
    UNKNOWN_TYPE,
    SHORT_TYPE,
    LONG_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE
} ; typedef char DATA_TYPE;


//  structure for passing information to functions that create a new matrix
//  unspecified elements inherit from other sources
typedef struct _MAT_INFO
{
    char            szFileName[_MAX_PATH];  //  "" = unspecified
    char            szLabel[_MAX_FLABEL];   //  "" = unspecified
    DATA_TYPE       Type;                   //  UNKNOWN_TYPE = unspecified
    CONTROL_TYPE    Sparse;                 //  |
    CONTROL_TYPE    ColMajor;               //  | CONTROL_AUTOMATIC = unspecified
    CONTROL_TYPE    FileBased;              //  |
    long            Compression;            //  0 = None (default), 1 = Compressed
    long            bSparseOmitZeros;       //  In sparse mtx: 1 means the omitted entries are zeros; 0 means they're missing (default)
} MAT_INFO;



#if defined(__cplusplus)
extern "C" {
#endif 


// Initializes the matrix dll.  This function must
// be called before using any other function in the DLL.
// The program_tc_status variable will be set to the have
// the exit status of every DLL function called.
void    InitMatDLL(int *program_tc_status);

/*
 ** MATRIX_GetInfo
 *
 *  PARAMETERS:
 *      hMatrix    - matrix handle
 *      Info    - pointer to structure to fill
 *
 *  DESCRIPTION:
 *      Fills a matrix info structure based on 'hMatrix'.
 *
 *  RETURNS:
 *      Nothing.
 *
 */
void MATRIX_GetInfo(TC_MATRIX hMatrix, MAT_INFO *Info);


// Decrements the matrix reference count and closes the matrix if the 
//      reference count is zero
short   MATRIX_Done(TC_MATRIX  hMatrix);

//  Read a matrix from a file.
//  The last argument can be used to force the loading mode of a dense matrix.
//  If it is passed as CONTROL_AUTOMATIC, the mode is read from the file.
//  If the stored file was created in memory, it will be read into memory.
//  A file-based matrix remains in the file (only the header is read).
TC_MATRIX  MATRIX_LoadFromFile(char *szFileName, CONTROL_TYPE FileBased);
    
// Returns the matrix file name.
void    MATRIX_GetFileName(TC_MATRIX  hMatrix, char *szFileName);

// Returns the number of dim indices
short MATRIX_GetNIndices(TC_MATRIX m, MATRIX_DIM dim);

//  Returns the number of cores in the matrix.
short   MATRIX_GetNCores(TC_MATRIX  hMatrix);
    
//  Returns the number of rows in the matrix.
long    MATRIX_GetNRows(TC_MATRIX  hMatrix);
    
//  Returns the number of columns in the matrix.
long    MATRIX_GetNCols(TC_MATRIX  hMatrix);
    
//  Returns the number of rows in the matrix core.
long    MATRIX_GetBaseNRows(TC_MATRIX hMatrix);
    
//  Returns an array of index IDs for the matrix.
short   MATRIX_GetIDs(TC_MATRIX hMatrix, MATRIX_DIM dim,  long *ids);

//  Returns the number of columns in the matrix core.
long    MATRIX_GetBaseNCols(TC_MATRIX hMatrix);
    
//  Returns the data type of the elements in the matrix.
DATA_TYPE   MATRIX_GetDataType(TC_MATRIX hMatrix);
    
//  Tests whether the matrix is in column-major order.
int    MATRIX_IsColMajor(TC_MATRIX hMatrix);
    
//  Sets the internal currency to the specified core.
short MATRIX_SetCore(TC_MATRIX hMatrix, short iCore);
    
//  Gets the index of the current core.
short MATRIX_GetCore(TC_MATRIX hMatrix);
    
//  Fills p with the value of the cell at (idRow, idCol) converted to Type.
short MATRIX_GetElement(TC_MATRIX hMatrix, long idRow, long idCol, DATA_TYPE Type, void *p);
    
//  Reads a row or column of a matrix using the base index.
//  Arguments:
//      iPos    -   the core position of the row or the column.
//      dim     -   dimension to extract: MATRIX_ROW or MATRIX_COL.
//      Type    -   Requested data type of returned values.
//      Array   -   Previously allocated array for receiving values.
short MATRIX_GetBaseVector(TC_MATRIX hMatrix, long iPos, MATRIX_DIM dim, DATA_TYPE Type, void *Array);

short MATRIX_GetVector(TC_MATRIX hMatrix, long ID, MATRIX_DIM dim, DATA_TYPE Type, void *Array);
//  Reads a row or column of a matrix using the current index.
//      ID      -   the identifier of the row or the column.
//      dim     -   dimension to extract: MATRIX_ROW or MATRIX_COL.
//      Type    -   Requested data type of returned values.
//      Array   -   Previously allocated array for receiving values.

//  Sets an element of the matrix 
short MATRIX_SetElement(TC_MATRIX hMatrix, long idRow, long idCol, DATA_TYPE Type, void *p);
 
//  Sets a row or column of a matrix using the base index.
//  Arguments:
//      iPos    -   the core position of the row or the column.
//      dim     -   dimension to set: MATRIX_ROW or MATRIX_COL.
//      Type    -   data type of provided values.
//      Array   -   array of values to store.
short MATRIX_SetBaseVector(TC_MATRIX hMatrix, long iPos, MATRIX_DIM dim, DATA_TYPE Type, void *Array);
    
//  Sets a row or a column  of a matrix using the current index.
//  Arguments:
//      ID      -   the identifier of the row or the column.
//      dim     -   dimension to set: MATRIX_ROW or MATRIX_COL.
//      Type    -   data type of provided values.
//      Array   -   array of values to store.
short MATRIX_SetVector(TC_MATRIX hMatrix, long ID, MATRIX_DIM dim, DATA_TYPE Type, void *Array);

// Returns the position of the current index.
short MATRIX_GetCurrentIndexPos(TC_MATRIX hMatrix, MATRIX_DIM dim);

//  Sets the current matrix index for the the requested dimension.
short MATRIX_SetIndex(TC_MATRIX hMatrix, MATRIX_DIM dim, short iIdx);
    
//  For file-based matrices - creates a cache, not greater than nSize bytes.
//  Type is either SERIAL_CACHE or RANDOM_CACHE.
//  The serial cache is only useful for a sequential read of the
//      matrix, in natural (base) order.
//  The random cache is only useful for random access where each element
//      is accessed several times.
//  Apply is either CACHE_ONE or CACHE_ALL.  In case of CACHE_ONE - the
//      cache is created for the current core, and every time the core is
//      changed (via MATRIX_SetCore), the old cache is destroyed and a
//      new one is created.  For CACHE_ALL - a separate cache is created
//      for each core, the actual size determined by the number of cores.
//  Note: This should be called in pairs with MATRIX_DestroyCache().
//      Nested calls are allowed.
short MATRIX_CreateCache(TC_MATRIX hMatrix, MAT_CACHE_TYPE Type, MAT_CACHE_APPLY apply, long nSize);

// Destroys a matrix cache.
void    MATRIX_DestroyCache(TC_MATRIX hMatrix);
    
//  Disables the use of the cache.  
short MATRIX_DisableCache(TC_MATRIX hMatrix);
    
//  Enables the cache.  
short MATRIX_EnableCache(TC_MATRIX hMatrix);

//  Reads back the cache buffer(s) for a matrix.
short MATRIX_RefreshCache(TC_MATRIX hMatrix);

//  Fills szLabel with the label of the specified core.
void    MATRIX_GetLabel(TC_MATRIX hMatrix, short iCore, char *szLabel);

// Set the label for a specified core.
void MATRIX_SetLabel(TC_MATRIX hMatrix, short iCore, char *szLabel);

//  Tests whether the matrix is in sparse representation.
BOOL    MATRIX_IsSparse(TC_MATRIX hMatrix);

//  Tests whether the matrix is in file-based.
BOOL    MATRIX_IsFileBased(TC_MATRIX hMatrix);

// Increases the reference count of the matrix. This call should be used to increase access speed.
// It must be called in pair with MATRIX_CloseFile.
short MATRIX_OpenFile(TC_MATRIX hMatrix, BOOL fRead);

// Decreases the reference count of the matrix.
short MATRIX_CloseFile(TC_MATRIX hMatrix);

// Clears the contents of the matrix.
short MATRIX_Clear(TC_MATRIX hMatrix);

// Checks whether the matrix is read only.
BOOL MATRIX_IsReadOnly(TC_MATRIX hMatrix);

/*
 * Matrix_New
 *
 * PARAMETERS:
 *      file_name - file name full path where the matrix should be stored
 *      Label - matrix label name
 *      n_rows - number of rows
 *      row_ids - null for natural order base index, or an array of size n_rows with the IDs for each row.
 *      n_cols - number of cols
 *      col_ids - null for natural order base index, or an array of size n_rows with the IDs for each row.
 *      n_cores - the number of matrix cores
 *      core_names - core_name[i] = name of core matrix i.  if core_name is null then
 *              core names are: "Matrix 1", "Matrix 2", ... , "Matrix N".
 *      data_type - numeric data type to store in the matrix (SHORT_TYPE, LONG_TYPE, FLOAT_TYPE or DOUBLE_TYPE)
 *      compression - 0 if no compression, 1 if matrix should use compression.
 *
 *  DESCRIPTION:
 *      Creates a new matrix file.
 *
 * RETURNS:
 *      New matrix handle
 *
 */
TC_MATRIX MATRIX_New(char *file_name, char *Label, long n_rows, long *row_ids, long n_cols, long *col_ids, long n_cores, char **core_names,DATA_TYPE data_type, short compression);



/*
 ** MATRIX_Copy
 *
 *  PARAMETERS:
 *      hSource - source matrix
 *      Info    - matrix creation parameters
 *      iCore   - core to copy.  Use -1 to copy all cores.
 *      CopyAllIndices   - True for copy base and all indices, False for copy current
 *                index only (making it into base of new matrix). 
 *      copy_cores - an array with the indeces of the cores to copy, or null to copy all cores.
 *                         e.g., copy_cores[i] = core index to copy
 *      nCopyCores - -1 to copy all cores, else the number of cores to copy (the size of the copy_cores array)
 *
 *
 *  DESCRIPTION:
 *      Copy one or more cores to a new matrix.
 *
 *  RETURNS:
 *      New matrix handle.
 *
 */
TC_MATRIX MATRIX_Copy(TC_MATRIX hSource, MAT_INFO *Info, short iCore, BOOL CopyAllIndices, long *copy_cores, long nCopyCores);




#if defined(__cplusplus)
}
#endif

#endif  //  !defined(CALIPERMTX_H)
