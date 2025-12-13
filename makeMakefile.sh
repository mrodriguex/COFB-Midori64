#!/bin/sh

### OBTIENE TODA LA ENTRADA ESTANDAR
args=$(echo $@ | tr [:space:] ',' | tr '-' '\n')

#######################################################
### PROCESAR LA ENTRADA DE LOS ARGUMENTOS #############
#######################################################

### PARA CADA ARGUMENTO DE LA ENTRADA ESTANDAR ########
for arg in $args
do
### OBTIENE LOS PARAMETROS DEL ARGUMENTO
params=$(echo $arg | tr ',' ' ')
orden="vacio"

	### PARA CADA PARAMETRO DEL ARGUMENTO
	for param in $params
	do

	### SI NO SE HABIA LEIDO LA ORDEN
	if [ $orden = "vacio" ]; then
		orden="$param"
	else		
	### SI SE TIENE LA ORDEN
		case $orden in 
		### CONCATENA SUS PARAMETROS
			"a")
				argsA=$argsA"$param ";;
			"i")
				argsI=$argsI"$param ";;
			"o")
				argsO=$argsO"$param ";;
			"b")
				argsB=$argsB"$param ";;
			"c")
				argsC=$argsC"$param ";;
			*)
			args_=$args_"-$orden $param ";;
		esac
	fi
	
	done
done
#######################################################


#######################################################
### VALIDACIONES DE LOS ARGUMENTOS Y SUS PARAMETROS ###
#######################################################

### ARGUMENTOS ADICIONALES NO VALIDOS
if [ -n "$args_" ]; then
	echo "Error: los argumentos [ "$args_"] no son válidos"
	exit
fi
#######################################################


### VERIFICA DIRECTORIO DESTINO DEL EJECUTABLE ########
numArgsA=$(echo $argsA | wc -w)
if [ "$numArgsA" -eq 1 ]; then
	if [ ! -f $argsA ]; then
		echo "Error: el archivo principal [$argA] no es válido"
		exit
	fi
else
	argsA="./"
fi
#######################################################

### VERIFICA DIRECTORIO ORIGEN DE LAS CABECERAS #######
numArgsI=$(echo $argsI | wc -w)
if [ "$numArgsI" -gt 0 ]; then
	for argI in $argsI 
	do
		if [ ! -d $argI ]; then
			echo "Error: el directorio de cabeceras [$argI] no es válido"
		exit
		fi
	done
else
	argsI="./"
fi
#######################################################

### VERIFICA DIRECTORIO DESTINO DE LOS OBJETOS ########
numArgsO=$(echo $argsO | wc -w)
if [ "$numArgsO" -eq 1 ]; then
	if [ ! -d $argsO ]; then
		echo "Advertencia: [ $argsO] no existe, pero se creará"
		#crear directorio
		mkdir $argsO
	fi
else
	argsO="./"
fi
#######################################################

### VERIFICA DIRECTORIO ORIGEN DE LAS FUENTES #########
numArgsC=$(echo $argsC | wc -w)
if [ "$numArgsC" -gt 0 ]; then
	for argC in $argsC 
	do
		if [ ! -d $argC ]; then
			echo "Error: el directorio de fuentes [$argC] no es válido"
		exit
		fi
	done
else
	argsC="./"
fi
#######################################################

### VERIFICA DIRECTORIO DESTINO DE LOS BINARIOS #######
numArgsB=$(echo $argsB | wc -w)
if [ "$numArgsB" -eq 1 ]; then
	if [ ! -f $argsB ]; then
		echo "Advertencia: [ $argsB ] no existe, pero se creará"
	fi
else
	argsB="a.out"
fi
#######################################################


#######################################################
### OBTENER INCLUDES DE LAS FUENTES ###################
#######################################################

preparaBusqueda ()
{
src1=$1
baseSrc=`basename $src`
baseSrcO=`echo $baseSrc | sed "s/.c$/.o/g"`
allObj="$allObj $baseSrcO"
libs=""
buscaDep $src
allDirs="$allDirs$libs"
libsX="\n\$(OBJ_DIR)/$baseSrcO: "
libsY=""
for li in $libs
do
	libsY="$libsY $li"
done
libsZ="\n\t\$(CC) \$(PROF) \$(FLAGS_CC) -o \$(OBJ_DIR)/$baseSrcO \$(INCL_DIR) -c $src \n\t\$(COMMANDS) \n"
concat="$concat$libsX$libsY$libsZ"
}

buscaDep ()
{
src1=$1
libEnc=""
for libEnc in $libs
do
	if [ "$src1" = "$libEnc" ]; then
	libEnc="encontrado"
	break
	fi
done
if [ ! "$libEnc" = "encontrado" ] ; then
	libs="$libs $src1"
	if [  -f $src1 ]; then
		local dire=`dirname $src1`
		local arch=`basename $src1`
		local comm="cat $dire/$arch"
		incS=$($comm | sed "s/ //g" | grep -E "#include\"" | sed 's/"//g' | sed "s/#include//g")
		for inc in $incS
		do
			for direc in $argsI
			do
				direc=$(echo $direc | sed 's/\/$//g')
				busca="$direc/$inc"
				busca=$(echo $busca | sed "s/^.\///g")
 				if [  -f $busca ]; then
					buscaDep $busca
				fi
			done
		done
	fi
fi
}

### COMIENZA A IMPRIMIR MAKEFILE ###
argO=$(echo $argsO  | sed 's/ $//g')
concat="\nAUTO : $argsB"
concat=$concat"\nOBJ_DIR = $argO"
concat=$concat"\nINCL_DIR = "

for ldir in $argsI
do
	ldir=$(echo $ldir  | sed "s/^.\///g" | sed "s/\/$//g")
	concat=$concat"-I$ldir "
done
concat=$concat"\n"

### BUSCA ENCABEZADOS EN LAS FUENTES ##################
for argA in $argsA 
do
	srcS=`find $argA -maxdepth 1 -iname "*.c" -print`
	for src in $srcS
	do
		if [ ! -d $src ]; then
			src=$(echo $src | sed "s/^.\///g")
			preparaBusqueda $src
		fi
	done
done
#######################################################

### BUSCA ENCABEZADOS EN LAS FUENTES ##################
for argC in $argsC 
do
	srcS=`find $argC -maxdepth 1 -iname "*.c" -print`
	for src in $srcS
	do
		if [ ! -d $src ]; then
			src=$(echo $src | sed "s/^.\///g")
			preparaBusqueda $src
		fi
	done
done
#######################################################

### CONTINUA IMPRIMIENDO MAKEFILE ###
concat=$concat"\nALL_OBJ = "
for obj in $allObj
do
	concat=$concat"\$(OBJ_DIR)/$obj "
done
concat=$concat"\n\n$argsB : \$(ALL_OBJ)\n"
concat=$concat"\tcc $FLAGS_CC -o $argsB \$(ALL_OBJ)"
echo $concat > Makefile
#######################################################
