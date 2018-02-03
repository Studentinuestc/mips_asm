#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<string.h>
#include<vector>
#include<map>
#include<set>
#include<istream>
#include<fstream>
#include<bitset>
#include<stdio.h>
using namespace std;


/**
*	@brief ��ʱֻ֧��mips���תΪ�����룬֧��ע�ͣ�
*	֧���������mips�������׳�
*	ͬʱĬ�ϱ������ȡ�Ļ������ļ��еĵ�һ�������ַΪ0x00000000
*	��֧�� .text .data main:�ȵ�
*	
*
*	�������µĴ����ܸ���������
*	lw   $t0,8($s4) #dfs  
*	tes1: lw  $t1 , 0 ( $s1 ) #fsfs   
*	add $t0, $t0,  $t1 #fefsf  
*	test2: sw  $t0, 12  ( $s4)   #fgsdg  
*	sub $k0  ,$a1,$a2  #grewgres  
*	slt $s0,  $s2,$s1   #fdsgs  
*	beq $k0,$k1,   tes1 #fgsgsd   
*	j   test2 #regfs  
*	beq $k0,$k1, 12345 #fdsgsdfsdgf  
*	add $s1,$s2,$s3  
*	sub $s1,$s2,$s3 #gfsgfdgs  
*	slt $s1,$s2,$s3  
*	lw $s1,20($s2)  #ddsfgrwad  
*	hello: lw $s1,-20($s2)  
*	sw $s1,20($s2)  
*	sw $s1,-20($s2) #fdsgsdgdfs  
*	beq $s0,$s1,25  
*	j 250  
*	beq $s0,$s1,for  
*	for: j hello #rgegdsgfds  
*/
class Mips_asm
{
public:
	/**
	* @param file_name:������ļ���
	*/
	Mips_asm()
	{
		result = "";
		ins_num = 0;
	}

	~Mips_asm()
	{

	}
	/**
	* @param in_file_name:����ı����������ļ���
	*	out_file_name����������������ļ���
	*/
	string core(const string in_file_name,string out_file_name="������.txt")
	{
		//��ȡ�ļ�
		ifstream file_read(in_file_name);
		//���ļ����ɴ�ʱ
		if (!file_read.is_open())
		{
			cout << "�ļ��޷���" << endl;
			return "";
		}
		//һֱ��ȡ���ļ���β
		while (!file_read.eof())
		{
			string temp;
			getline(file_read, temp);
			///��ȡ������ʱ
			if (temp=="")
			{
				continue;
			}
			vector<string>toke = _split(temp, " ,()");
			for (size_t i = 0; i <toke.size(); i++)
			{
				///��ȡ��ע��
				if (toke[i][0] == '#')
				{
					break;
				}
				///��ȡ����ת������,��ת��������ͬ�Ļ����׳�����
				else if (toke[i].back() == ':')
				{
					
					toke[i].pop_back();
					if (jump_line.find(toke[i])==jump_line.end())
					{
						jump_line.insert(make_pair(toke[i], ins_num + 1));
					}
					else
					{
						//throw ("dfds");
					}
				}
				///ָ��Ϊr��ʱ
				else if (r_type.find(toke[i]) != r_type.end())
				{
					ins_num++;
					type_val val = r_type.find(toke[i])->second;
					result.append(val.op);
					///Ϊ�����߼�ָ��ʱ
					if (val.rs == ""&&val.shamt=="00000")
					{
						result.append(_get_reg_addr(toke[i + 2]));
						result.append(_get_reg_addr(toke[i + 3]));
						result.append(_get_reg_addr(toke[i + 1]));
						result.append(val.shamt);
						result.append(val.func);
					}
					///Ϊ������λָ��ʱ
					else if(val.rs == "00000")
					{
						result.append(val.rs);
						result.append(_get_reg_addr(toke[i + 2]));
						result.append(_get_reg_addr(toke[i + 1]));
						result.append(_o_to_b(toke[i + 3],5));
						result.append(val.func);
					}
					///Ϊ������λָ��ʱ
					else
					{
						result.append(_get_reg_addr(toke[i + 3]));
						result.append(_get_reg_addr(toke[i + 2]));
						result.append(_get_reg_addr(toke[i + 1]));
						result.append(val.shamt);
						result.append(val.func);

					}
					i += 3;
				}
				///ָ��Ϊi��ʱ
				else if (i_type.find(toke[i]) != i_type.end())
				{
					ins_num++;
					type_val val = i_type.find(toke[i])->second;
					result.append(val.op);

					//TODO
					///Ϊbeqʱ��ΪΪbneʱ
					if (val.op=="000100"||val.op=="000101")
					{
						result.append(_get_reg_addr(toke[i + 2]));
						result.append(_get_reg_addr(toke[i + 1]));
						if (jump_line.find(toke[i + 3])==jump_line.end())
						{
							result.append(_o_to_b(toke[i + 3], 16));
						}
						else
						{
							int temp = (jump_line.find(toke[i + 3])->second) * 4;
							result.append(_o_to_b(to_string(temp), 16));
						}
						i += 3;
					}
					///Ϊluiʱ
					else if(val.op=="001111")
					{
						result.append(val.rs);
						result.append(_get_reg_addr(toke[i + 1]));
						result.append(_o_to_b(toke[i + 2], 16));
						i += 2;
					}
					///Ϊlw sw
					else if (val.op == "101011"|| val.op == "100011")
					{
						result.append(_get_reg_addr(toke[i + 3]));
						result.append(_get_reg_addr(toke[i + 1]));
						result.append(_o_to_b(toke[i + 2], 16));
						i += 3;
					}
					else
					{
						result.append(_get_reg_addr(toke[i + 2]));
						result.append(_get_reg_addr(toke[i + 1]));
						result.append(_o_to_b(toke[i + 3], 16));
						i += 3;
					}
				}
				///ָ��Ϊj��ʱ
				else if (j_type.find(toke[i]) != j_type.end())
				{
					ins_num++;
					type_val val = j_type.find(toke[i])->second;
					result.append(val.op);
					if (jump_line.find(toke[i + 1]) == jump_line.end())
					{
						result.append(_o_to_b(toke[i + 1], 26));
					}
					else
					{
						int temp = (jump_line.find(toke[i + 1])->second) * 4;
						result.append(_o_to_b(to_string(temp), 26));
					}
					i += 1;
				}
				///ָ��Ϊjr��ʱ
				else if (true)
				{
					ins_num++;
					type_val val = i_type.find(toke[i])->second;
					result.append(val.op);
					result.append(_o_to_b(toke[i + 1], 26));
					i += 1;
				}
				else
				{
					//throw("����");
				}
			}
			
		}
		file_read.close();
		_print_result();
		return result;
	}

private:
	/**
	*	@param s:�����ַ���
	*	seperator���ָ���
	*	@return �ַ���vector
	*	@brief ʵ���ַ����ָ�ܣ�֧�ֶ�ָ���.....
	*	Ϊɶc++û��ԭ�����ַ����ָ��
	*/
	vector<string> _split(const string &s, const string &seperator) 
	{
		vector<string> result;
		typedef string::size_type string_size;
		string_size i = 0;

		while (i != s.size()) 
		{
			//�ҵ��ַ������׸������ڷָ�������ĸ��
			int flag = 0;
			while (i != s.size() && flag == 0) 
			{
				flag = 1;
				for (string_size x = 0; x < seperator.size(); ++x)
					if (s[i] == seperator[x])
					{
						++i;
						flag = 0;
						break;
					}
			}

			//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
			flag = 0;
			string_size j = i;
			while (j != s.size() && flag == 0) 
			{
				for (string_size x = 0; x < seperator.size(); ++x)
					if (s[j] == seperator[x]) 
					{
						flag = 1;
						break;
					}
				if (flag == 0)
					++j;
			}
			if (i != j) 
			{
				result.push_back(s.substr(i, j - i));
				i = j;
			}
		}
		return result;
	}

	/**
	*	@brief	���ض�Ӧ�Ĵ����Ķ����Ƶ�ַ�ַ���
	*	�Ĵ���������or����ʱ,�׳��쳣
	*/
	string _get_reg_addr(string reg)
	{
		auto p = reg_addr.find(reg);
		if (p==reg_addr.end())
		{
			throw "�Ĵ�����������";
		}
		else
		{
			return p->second;
		}
	}

	/**
	*	@brief	ʮ����ת�������ַ���
	*	����bitsetʵ��10����ת�������ַ���
	*/
	string _o_to_b(string s,int a)
	{
		int temp = atoi(s.data());
		///����bitsetʵ��10����ת�������ַ���
		bitset<32> bs(temp);
		string pp = bs.to_string();
		return pp.substr(32-a, a);
	}

	void _print_result()
	{
		//��32λ�Ķ������ַ���תΪ16����
		string result_temp = "";
		for (size_t i = 0; i < result.size(); i += 4)
		{
			string temp = result.substr(i, 4);
			int t = strtol(temp.data(), NULL, 2);
			char str[5];
			sprintf(str, "%x", t);
			result_temp.append(str);
			//ÿ32λ�Ͽ�
			if (((i)%32+4)%32==0)
			{
				result_temp.append("\n");
			}
		}
		result = result_temp;
	}
private:
	///���
	string result;
	///�������������
	int ins_num;

	/**
	* R��ָ��
	*/
	struct type_val
	{
		string op;
		string rs;
		string shamt;
		string func;
	};
	type_val add = { "000000","","00000","100000" };
	type_val sub = { "000000","","00000","100010" };
	type_val and = { "000000","","00000","100100" };
	type_val or = { "000000","","00000","100101" };
	type_val xor = { "000000","","00000","100110" };
	type_val nor = { "000000","","00000","100111" };
	type_val slt = { "000000","","00000","101010" };
	type_val sll = { "000000","00000","","000000" };
	type_val srl = { "000000","00000","","000010" };
	type_val sra = { "000000","00000","","000011" };
	const map<string, type_val>r_type = {
		make_pair("add",add),
		make_pair("sub",sub),
		make_pair("and",and),
		make_pair("or", or),
		make_pair("xor",xor),
		make_pair("nor", nor),
		make_pair("slt", slt),
		make_pair("sll", sll),
		make_pair("srl", srl),
		make_pair("sra", sra),
	};

	/**
	* i��ָ��
	*/
	type_val addi = { "001000","","","" };
	type_val andi = { "001100","","","" };
	type_val ori = { "001101","","","" };
	type_val xori = { "001110","","","" };
	type_val lui = { "001111","00000","","" };
	type_val lw = { "100011","","","" };
	type_val sw = { "101011","","","" };
	type_val beq = { "000100","","","" };
	type_val bne = { "000101","","","" };
	type_val slti = { "001010","","","" };
	const map<string, type_val>i_type = {
		make_pair("addi",addi),
		make_pair("andi",andi),
		make_pair("ori",ori),
		make_pair("xori",xori),
		make_pair("lui",lui),
		make_pair("lw",lw),
		make_pair("sw",sw),
		make_pair("beq",beq),
		make_pair("bne",bne),
		make_pair("slti",slti),
	};

	/**
	* j��ָ��
	*/
	type_val j = { "000010","","","" };
	type_val jal = { "000011","","","" };
	const map<string, type_val>j_type = {
		make_pair("j",j),
		make_pair("jal",jal),
	};
	


	///�����Ĵ�����Ӧ�ĵ�ַ
	map <string, string >reg_addr = {
		make_pair("$0", "00000"),
		make_pair("$1", "00001"),
		make_pair("$2", "00010"),
		make_pair("$3", "00011"),
		make_pair("$4", "00100"),
		make_pair("$5", "00101"),
		make_pair("$6", "00110"),
		make_pair("$7", "00111"),
		make_pair("$8", "01000"),
		make_pair("$9", "01001"),
		make_pair("$10","01010"),
		make_pair("$11","01011"),
		make_pair("$12","01100"),
		make_pair("$13","01101"),
		make_pair("$14","01110"),
		make_pair("$15","01111"),
		make_pair("$16","10000"),
		make_pair("$17","10001"),
		make_pair("$18","10010"),
		make_pair("$19","10011"),
		make_pair("$20","10100"),
		make_pair("$21","10101"),
		make_pair("$22","10110"),
		make_pair("$23","10111"),
		make_pair("$24","11000"),
		make_pair("$25","11001"),
		make_pair("$26","11010"),
		make_pair("$27","11011"),
		make_pair("$28","11100"),
		make_pair("$29","11101"),
		make_pair("$30","11110"),
		make_pair("$31","11111"),
		make_pair("$32","10000"),

		make_pair("$zero","00000"),
		make_pair("$at","00001"),
		make_pair("$v0","00010"),
		make_pair("$v1","00011"),
		make_pair("$a0","00100"),
		make_pair("$a1","00101"),
		make_pair("$a2","00110"),
		make_pair("$a3","00111"),
		make_pair("$t0","01000"),
		make_pair("$t1","01001"),
		make_pair("$t2","01010"),
		make_pair("$t3","01011"),
		make_pair("$t4","01100"),
		make_pair("$t5","01101"),
		make_pair("$t6","01110"),
		make_pair("$t7","01111"),
		make_pair("$s0","10000"),
		make_pair("$s1","10001"),
		make_pair("$s2","10010"),
		make_pair("$s3","10011"),
		make_pair("$s4","10100"),
		make_pair("$s5","10101"),
		make_pair("$s6","10110"),
		make_pair("$s7","10111"),
		make_pair("$t8","11000"),
		make_pair("$t9","11001"),
		///26,27�żĴ��������쳣
		make_pair("$k0","11010"),
		make_pair("$k1","11011"),
		make_pair("$gp","11100"),
		make_pair("$sp","11101"),
		make_pair("$fp","11110"),
		make_pair("$ra","11111") };
	///��ת�����ƶ�Ӧ��������׼ȷ��˵Ӧ���Ƕ�Ӧ������������
	map<string, int>jump_line;
};


int ttt(int a)
{
	if (a==1)
	{
		throw a;
	}
}

int main()
{
	Mips_asm test;
	cout << test.core("test.txt") << endl;
	/*
	string s = "10";
	int temp = atoi(s.data());
	///����bitsetʵ��10����ת�������ַ���
	bitset<32> bs(temp);
	string pp= bs.to_string();
	int a = 5;
	cout << pp.substr(32-a, a) << endl;
	*/
	/*
	string tt = "0011";
	long i = strtol(tt.data(), NULL, 16);
	cout << i << endl;
	*/
	system("pause");
}